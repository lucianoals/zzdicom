const char *sqlinit =
"BEGIN TRANSACTION;"
"CREATE TABLE IF NOT EXISTS instances(filename text primary key not null, sopclassuid text, instanceuid text unique, size int default -1, lastmodified text not null, seriesuid text, "
"	volume int default 0, splitcause text default null);"
"CREATE TABLE IF NOT EXISTS series(seriesuid text primary key not null, modality text, seriesid text, studyuid text, annotation text default null);"
"CREATE TABLE IF NOT EXISTS studies(studyuid text primary key not null, patientsname text, accessionnumber text, birthdate text, patientsid text);"
"COMMIT;"
;
