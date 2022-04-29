# Traffic lights

## How to setup for build

- create and `cd` into build directory
- run `setup.sh` from source directory

`setup.sh` saves paths to source and build directories and
creates `Makefile`. Then it runs `make all`.

## How to build (after setup)

- `make back` (default) builds back end
- `make front` copies front end files
- `make all` builds back end + copies front end files

## Command and response frames

### Command (sent by SPI master):

| Size   | Name       | Description                                               |
|--------|------------|-----------------------------------------------------------|
| uint8  | frame_id   | Frame id counter. Increased by one on each transaction.   |
| uint8  | checksum   | Frame checksum.                                           |
| uint8  | reserved   | Reserved and unused.                                      |
| uint8  | command_id | Command / request id. Determines content of 'data' field. |
| uint64 | data       | Command data. Format depends on 'command_id'.             |

#### Command IDs (command_id field):

| Value | Description         |
|-------|---------------------|
| 0     | Unused              |
| 1     | Lane states request |
| 2     | Debug               |
| 3     | Set parameter value |
| 4     | Get parameter value |

#### Lane states request command

Contents of the 'data' field in command when 'command_id' equals to '1' (Lane state request)

| Byte (LSB) | Description                |
|------------|----------------------------|
| 0          | Right lane state request.  |
| 1          | Center lane state request. |
| 2          | Left lane state request.   |
| 3 - 7      | Reserved.                  |

Lane state request values:

| Value | Description                          |
|-------|--------------------------------------|
| 0     | Unused                               |
| 1     | Stop (Red light on)                  |
| 2     | Go (Green light)                     |
| 3     | Out of order (Yellow light blinking) |

### Response (transmitted by SPI slave):

| Size   | Name        | Description                                                      |
|--------|-------------|------------------------------------------------------------------|
| uint8  | frame_id    | Frame id counter. Latest 'frame_id' received in a command frame. |
| uint8  | checksum    | Frame checksum. Unimplemented.                                   |
| uint8  | reserved    | Reserved and unused.                                             |
| uint8  | response_id | Response id. Determines content of 'data' field.                 |
| uint64 | data        | Command data. Format depends on 'response_id'.                   |

#### Response IDs (response_id field):

| Value | Description     |
|-------|-----------------|
| 0     | Unused          |
| 1     | Lane states     |
| 2     | Parameter value |

#### Lane states response

Contents of the 'data' field in response when 'response_id' equals to '1' (Lane states)

| Byte (LSB) | Description                                                   |
|------------|---------------------------------------------------------------|
| 0          | Right lane: low nibble: state, high nibble: cars on lane      |
| 1          | Center lane: low nibble: state, high nibble: cars on lane     |
| 2          | Left lane state: low nibble: state, high nibble: cars on lane |
| 3 - 7      | Reserved.                                                     |

Lane state values:

| Value | Description                              |
|-------|------------------------------------------|
| 0     | None                                     |
| 1     | Stop (Red light on)                      |
| 2     | Transition to Go (Red + Yellow light on) |
| 3     | Go (Green light on)                      |
| 4     | Transition to Stop (Yellow light on)     |
| 5     | Out of order (Yellow light blinking)     |

Cars on lane values:

| Value | Description              |
|-------|--------------------------|
| 0     | No cars on lane          |
| 1     | One or many cars on lane |
