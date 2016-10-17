#include "cfldb.h"
#include "crc.h"

#include <stdlib.h>

#include <3ds/result.h>
#include <3ds/services/fs.h>
#include <3ds/types.h>

#define SHARED_EXTDATA_ID 0xf000000b
#define CFL_DB_PATH (u"/CFL_DB.dat")

#define RETURN_ON_FAILURE(expr)                                                \
    {                                                                          \
        Result _res = expr;                                                    \
        if (R_FAILED(_res)) {                                                  \
            return _res;                                                       \
        }                                                                      \
    }

static Result shared_extdata_open_archive(FS_Archive *const archive,
                                          u32 const id)
{
    u32 path[3];
    path[0] = MEDIATYPE_NAND;
    path[1] = id;
    path[2] = 0x00048000;

    FS_Path fs_path = {PATH_BINARY, 12, path};

    return FSUSER_OpenArchive(archive, ARCHIVE_SHARED_EXTDATA, fs_path);
}

static Result shared_extdata_close_archive(FS_Archive const archive)
{
    return FSUSER_CloseArchive(archive);
}

static Result shared_extdata_open_file(Handle *const file,
                                       FS_Archive const archive,
                                       u16 const *const path)
{
    u32 size     = 0;
    u16 const *p = path;
    while (*p++)
        size++;
    size = (size + 1) * 2;

    FS_Path fs_path = {PATH_UTF16, size, path};
    return FSUSER_OpenFile(
        file, archive, fs_path, FS_OPEN_READ | FS_OPEN_WRITE, 0x0);
}

static Result shared_extdata_close_file(Handle file)
{
    return FSFILE_Close(file);
}

Result cfldb_open(CFL_DB *const db)
{
    RETURN_ON_FAILURE(
        shared_extdata_open_archive(&db->archive, SHARED_EXTDATA_ID));

    RETURN_ON_FAILURE(
        shared_extdata_open_file(&db->fhandle, db->archive, CFL_DB_PATH));

    RETURN_ON_FAILURE(FSFILE_GetSize(db->fhandle, &db->size));

    db->data = NULL;

    return 0;
}

Result cfldb_close(CFL_DB *const db)
{
    free(db->data);
    db->data = NULL;

    RETURN_ON_FAILURE(shared_extdata_close_file(db->fhandle));
    db->fhandle = 0;

    return shared_extdata_close_archive(db->archive);
}

Result cfldb_read(CFL_DB *const db)
{
    db->data = malloc(db->size);
    if (db->data == NULL) {
        return CFLDB_ERR_NO_MEM;
    }

    u32 bytes_read = 0;
    RETURN_ON_FAILURE(
        FSFILE_Read(db->fhandle, &bytes_read, 0x0, db->data, db->size));

    if (bytes_read != db->size) {
        return CFLDB_ERR_WRONG_SIZE;
    }

    return 0;
}

Result cfldb_write(CFL_DB *const db)
{
    u32 bytes_written;
    RETURN_ON_FAILURE(FSFILE_Write(
        db->fhandle, &bytes_written, 0x0, db->data, db->size, 0x0));

    if (bytes_written != db->size) {
        return CFLDB_ERR_WRONG_SIZE;
    }

    return 0;
}

Result cfldb_dump_to_sdmc(CFL_DB const *const db, char const *const path)
{

    Handle h_dump;
    FS_Archive a_sdmc;
    Result res;

    res = FSUSER_OpenArchive(&a_sdmc, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
    if (R_FAILED(res)) {
        goto failure;
    }

    res = FSUSER_OpenFileDirectly(&h_dump,
                                  ARCHIVE_SDMC,
                                  fsMakePath(PATH_EMPTY, ""),
                                  fsMakePath(PATH_ASCII, path),
                                  FS_OPEN_WRITE | FS_OPEN_CREATE,
                                  0);
    if (R_FAILED(res)) {
        goto failure_close_archive;
    }

    u32 bytes_written;
    res = FSFILE_Write(h_dump, &bytes_written, 0, db->data, db->size, 0);
    if (R_FAILED(res)) {
        goto failure_close_file;
    }

    res = (bytes_written != db->size) ? CFLDB_ERR_WRONG_SIZE : 0;

failure_close_file:
    FSFILE_Close(h_dump);
failure_close_archive:
    FSUSER_CloseArchive(a_sdmc);
failure:
    return res;
}

u16 cfldb_get_checksum(CFL_DB const *const db)
{
    u16 checksum = db->data->checksum;
    // Checksum is stored in big-endian format
    return (checksum << 8) | (checksum >> 8);
}

void cfldb_set_checksum(CFL_DB *const db, u16 const checksum)
{
    db->data->checksum = (checksum << 8) | (checksum >> 8);
}

u16 cfldb_calculate_checksum(CFL_DB const *const db)
{
    return crc_crc16_ccitt(db->data, offsetof(CFL_DB_Layout, checksum), 0x0000);
}

int cfldb_check_integrity(CFL_DB const *const db)
{
    u16 checksum   = cfldb_get_checksum(db);
    u16 calculated = cfldb_calculate_checksum(db);

    return checksum != calculated;
}

u16 cfldb_fix_checksum(CFL_DB *const db)
{
    u16 checksum = cfldb_calculate_checksum(db);
    cfldb_set_checksum(db, checksum);
    return checksum;
}

int cfldb_get_last_mii_index(CFL_DB const *const db)
{
    Mii *miis   = db->data->miimaker.miis;
    int last = CFLDB_MIIMAKER_MAX - 1;
    while (last >= 0 && !mii_is_valid(&miis[last])) {
        last--;
    }
    return last;
}

Mii *cfldb_get_mii_array(CFL_DB const *const db)
{
    CFL_DB_MiiMaker *miimaker = &db->data->miimaker;
    if (miimaker->magic == CFLDB_MIIMAKER_MAGIC) {
        return miimaker->miis;
    }

    return NULL;
}
