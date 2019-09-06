from SCons.Script import Builder
from os.path import join
import struct

Import("env")

def add_checksum(target, source, env):
    """Copy source to target, fixing up the checksum in 32-bit word 7
    """
    if len(target) != 1:
        return 1
    target = target[0]
    if len(source) != 1:
        return 1
    source = source[0]
    with open(str(source), 'rb') as f:
        data = bytearray(f.read())
    if len(data) < (8 * 4):
        # Too short
        return 1
    words = list(struct.unpack_from("I" * 7, data))
    csum = 0
    for i in words:
        csum = (csum + i) % 0xffffffff
    csum = ((~csum) + 1) & 0xffffffff
    words.append(csum)
    struct.pack_into("I" * 8, data, 0, *words)
    with open(str(target), 'wb') as f:
        f.write(data)

env.Append(
    BUILDERS={
        'CSumBin': Builder(
            action=add_checksum,
            suffix=".cbin",
            src_suffix=".bin"
        )
    }
)

target_csum = env.CSumBin(
    join("$BUILD_DIR", "${PROGNAME}"),
    join("$BUILD_DIR", "${PROGNAME}.bin"))
AlwaysBuild(env.Alias("csum", target_csum))
