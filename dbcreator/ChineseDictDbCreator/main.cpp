#include <stdio.h>
#include "dbcreator.h"

#define DB_CREATION_FILE "words.db"
#define DB_TARGET_FILE "words.db"
#define CEDICT_FILE "../../data/cedict_ts.u8"
#define WORD_RANK_FILE "../../data/internet-zh.num"

int main()
{
    createDb(DB_CREATION_FILE, CEDICT_FILE, WORD_RANK_FILE);

    return 0;
}
