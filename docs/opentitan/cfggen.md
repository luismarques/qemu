# `cfggen.py`

`cfggen.py` is a helper tool that can generate a QEMU OT configuration file,
for use with QEMU's `-readconfig` option, populated with sensitive data for
the ROM controller(s), the OTP controller, the Life Cycle controller, etc.

It heurastically parses configuration and generated RTL files to extract from
them the required keys, seeds, nonces and other tokens that are not stored in
the QEMU binary.

## Usage

````text
usage: cfggen.py [-h] [-o CFG] [-T TOP] [-c SV] [-l SV] [-t HJSON] [-s SOCID]
                 [-C COUNT] [-v] [-d]
                 TOPDIR

OpenTitan QEMU configuration file generator.

options:
  -h, --help            show this help message and exit

Files:
  TOPDIR                OpenTitan top directory
  -o CFG, --out CFG     Filename of the config file to generate
  -T TOP, --top TOP     OpenTitan Top name (default: darjeeling)
  -c SV, --otpconst SV  OTP Constant SV file (default: auto)
  -l SV, --lifecycle SV
                        LifeCycle SV file (default: auto)
  -t HJSON, --topcfg HJSON
                        OpenTitan top HJSON config file (default: auto)

Modifiers:
  -s SOCID, --socid SOCID
                        SoC identifier, if any
  -C COUNT, --count COUNT
                        SoC count (default: 1)

Extras:
  -v, --verbose         increase verbosity
  -d, --debug           enable debug mode
````


### Arguments

`TOPDIR` is a required positional argument which should point to the top-level directory of the
OpenTitan repository to analyze. It is used to generate the path towards the required files to
parse, each of which can be overidden with options `-c`, `-l` and `-t`.

* `-C` specify how many SoCs are used on the platform

* `-c` alternative path to the `otp_ctrl_part_pkg.sv` file

* `-d` only useful to debug the script, reports any Python traceback to the standard error stream.

* `-l` alternative path to the `lc_ctrl_state_pkg.sv.sv` file

* `-o` the filename of the configuration file to generate. It not specified, the generated content
  is printed out to the standard output.

* `-s` specify a SoC identifier for OT platforms with mulitple SoCs

* `-T` specify the OpenTitan _top_ name, such as `Darjeeling`, `EarlGrey`, ... This option is
  case-insensitive.

* `-t` alternative path to the `top_<top>.gen.hjson` file

* `-v` can be repeated to increase verbosity of the script, mostly for debug purpose.


### Examples

````sh
./scripts/opentitan/cfggen.py ../opentitan-integrated -o opentitan.cfg
````
