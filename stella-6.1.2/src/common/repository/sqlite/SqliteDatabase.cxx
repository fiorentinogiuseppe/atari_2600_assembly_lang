//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2020 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#include <cstdio>

#include "SqliteDatabase.hxx"
#include "Logger.hxx"
#include "SqliteError.hxx"

#ifdef BSPF_WINDOWS
  #define SEPARATOR "\""
#else
  #define SEPARATOR "/"
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqliteDatabase::SqliteDatabase(const string& databaseDirectory,
                               const string& databaseName)
  : myDatabaseFile(databaseDirectory + SEPARATOR + databaseName + ".sqlite3")
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqliteDatabase::~SqliteDatabase()
{
  if (myHandle) sqlite3_close_v2(myHandle);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteDatabase::initialize()
{
  if (myHandle) return;

  bool dbInitialized = false;

  for (int tries = 1; tries < 3 && !dbInitialized; tries++) {
    dbInitialized = (sqlite3_open(myDatabaseFile.c_str(), &myHandle) == SQLITE_OK);

    if (dbInitialized)
      dbInitialized = sqlite3_exec(myHandle, "PRAGMA schema_version", nullptr, nullptr, nullptr) == SQLITE_OK;

    if (!dbInitialized && tries == 1) {
      Logger::info("sqlite DB " + myDatabaseFile + " seems to be corrupt, removing and retrying...");

      remove(myDatabaseFile.c_str());
      if (myHandle) sqlite3_close_v2(myHandle);
    }
  }

  if (!dbInitialized) {
    if (myHandle) {
      string emsg = sqlite3_errmsg(myHandle);

      sqlite3_close_v2(myHandle);
      myHandle = nullptr;

      throw SqliteError(emsg);
    }

    throw SqliteError("unable to initialize sqlite DB for unknown reason");
  };

  Logger::debug("successfully opened " + myDatabaseFile);

  exec("PRAGMA journal_mode=WAL");

  switch (sqlite3_wal_checkpoint_v2(myHandle, nullptr, SQLITE_CHECKPOINT_TRUNCATE, nullptr, nullptr)) {
    case SQLITE_OK:
      break;

    case SQLITE_MISUSE:
      Logger::info("failed to checkpoint WAL on " + myDatabaseFile + " - WAL probably unavailable");
      break;

    default:
      Logger::info("failed to checkpoint WAL on " + myDatabaseFile + " : " + sqlite3_errmsg(myHandle));
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteDatabase::exec(const string& sql) const
{
  if (sqlite3_exec(myHandle, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
    throw SqliteError(myHandle);
}
