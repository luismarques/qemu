# Copyright (c) 2023-2024 Rivos, Inc.
# SPDX-License-Identifier: Apache2

"""Lifecycle helpers.

   :author: Emmanuel Blot <eblot@rivosinc.com>
"""

from binascii import unhexlify
from io import StringIO
from logging import getLogger
from os.path import basename
from re import finditer, match
from textwrap import fill
from typing import TextIO

from ot.util.misc import camel_to_snake_case


class OtpLifecycle:
    """Decoder for Lifecyle bytes sequences.
    """

    EXTRA_SLOTS = {
        'lc_state': {
            'post_transition': None,
            'escalate': None,
            'invalid': None,
        }
    }

    def __init__(self):
        self._log = getLogger('otp.lc')
        self._tables: dict[str, dict[str, str]] = {}
        self._tokens: dict[str, str] = {}

    def load(self, svp: TextIO):
        """Decode LifeCycle information.

           :param svp: System Verilog stream with OTP definitions.
        """
        ab_re = (r"\s*parameter\s+logic\s+\[\d+:\d+\]\s+"
                 r"([ABCD]\d+|ZRO)\s+=\s+\d+'(b(?:[01]+)|h(?:[0-9a-fA-F]+));")
        tbl_re = r"\s*Lc(St|Cnt)(\w+)\s+=\s+\{([^\}]+)\}\s*,?"
        codes: dict[str, int] = {}
        sequences: dict[str, list[str]] = {}
        svp = StringIO(svp.read())
        for line in svp:
            cmt = line.find('//')
            if cmt >= 0:
                line = line[:cmt]
            line = line.strip()
            abmo = match(ab_re, line)
            if not sequences and abmo:
                name = abmo.group(1)
                sval = abmo.group(2)
                val = int(sval[1:], 2 if sval.startswith('b') else 16)
                if name in codes:
                    self._log.error('Redefinition of %s', name)
                    continue
                codes[name] = val
                continue
            smo = match(tbl_re, line)
            if smo:
                kind = smo.group(1).lower()
                name = smo.group(2)
                seq = smo.group(3)
                items = [x.strip() for x in seq.split(',')]
                inv = [it for it in items if it not in codes]
                if inv:
                    self._log.error('Unknown state seq: %s', ', '.join(inv))
                if kind not in sequences:
                    sequences[kind] = {}
                sequences[kind][name] = items
                continue
        svp.seek(0)
        for tmo in finditer(r"\s+parameter\s+lc_token_t\s+(\w+)\s+="
                            r"\s+\{\s+128'h([0-9A-F]+)\s+\};",
                            svp.getvalue()):
            token, value = tmo.group(1), tmo.group(2)
            if token in self._tokens:
                raise ValueError(f'Multiple definitions of token {token}')
            self._tokens[token] = value.lower()
        for kind, seqs in sequences.items():
            mkind, conv = {'st': ('LC_STATE', str),
                           'cnt': ('LC_TRANSITION_CNT', int)}[kind]
            self._tables[mkind] = {}
            for ref, seq in seqs.items():
                seq = ''.join((f'{x:04x}'for x in map(codes.get, seq)))
                self._tables[mkind][seq] = conv(ref)

    def save(self, cfp: TextIO):
        """Save OTP life cycle definitions as a C file.

           :param cfp: output text stream
        """
        print(f'/* Section auto-generated with {basename(__file__)} '
              f'script */', file=cfp)
        for kind, table in self._tables.items():
            enum_io = StringIO()
            array_io = StringIO()
            count = len(table)
            length = max(len(x) for x in table.keys())//2
            print(f'static const char {kind.lower()}s[{count}u][{length}u]'
                  f' = {{', file=array_io)
            pad = ' ' * 8
            for seq, ref in table.items():
                if isinstance(ref, str):
                    slot = f'{kind}_{ref}'.upper()
                    print(f'    {slot},', file=enum_io)
                else:
                    slot = f'{ref}u'
                seqstr = ', '.join((f'0x{b:02x}u' for b in
                                    reversed(unhexlify(seq))))
                defstr = fill(seqstr, width=80, initial_indent=pad,
                              subsequent_indent=pad)
                print(f'    [{slot}] = {{\n{defstr}\n    }},',
                      file=array_io)
            print('};', file=array_io)
            for extra in self.EXTRA_SLOTS.get(kind.lower(), {}):
                slot = f'{kind}_{extra}'.upper()
                print(f'    {slot},', file=enum_io)
            enum_str = enum_io.getvalue()
            if enum_str:
                # likely to be moved to a header file
                print(f'enum {kind.lower()} {{\n{enum_str}}};\n', file=cfp)
            print(f'{array_io.getvalue()}', file=cfp)
        print('/* End of auto-generated section */', file=cfp)

    def get_configuration(self, name: str) -> dict[str, str]:
        """Provide a dictionary of configurable elements for QEMU."""
        return self._tables.get(name, {})

    def get_tokens(self, hashed: bool, zero: bool) -> dict[str, str]:
        """Return a dictionary of parsed tokens."""
        tokens = {}
        for token, value in self._tokens.items():
            sltoken = camel_to_snake_case(token)
            token_parts = sltoken.split('_')
            if token_parts[0] == 'rnd':
                token_parts.pop(0)
            if token_parts[0] == 'cnst':
                token_parts.pop(0)
            if not hashed and token_parts[-1] == 'hashed':
                continue
            if not zero and token_parts[-2] == 'zero':
                continue
            tkname = '_'.join(token_parts)
            tokens[tkname] = value
        return tokens
