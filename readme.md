# Env

- GCC 11.3
- Ubuntu 22.04 LTS
- MSI Modern 15 (i5 1155G7 - 16GB RAM)

# Prerequisites

Default maximum number of descriptor files can be opened in Ubuntu OS is 1024, we must increase this number in order to handle 5k.
To increase the limitation, run `ulimit -n 10000`

# Result

![Tested result](./screenshot.png "Benchmarked with simple node client")
