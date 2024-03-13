## This script is used to recreate the fill_chado_test_prepare.sql file used
## to simulate a chado prepare task being applied to the test schema more efficiently.
##
## Example Usage with TripalDocker named t4
## docker exec -it t4 bash modules/contrib/tripal/tripal_chado/tests/fixtures/regenerate_scripts/regenerate_fill_test_prepare.sh
##
## NOTE: This script uses an already prepared chado database to create the file.
## As such, make sure to use the UI to create a new Chado 1.3 schema named "preparedchado"
## and then use the UI to run the prepare task on it. Only then should you run this script.
##
dbuser='docker'
schemaname='preparedchado'
sitedb='sitedb'
file=modules/contrib/tripal/tripal_chado/tests/fixtures/fill_chado_test_prepare.sql

pg_dump --user $dbuser --no-owner --data-only --inserts --blobs --table=$schemaname'.cv*' --table=$schemaname'.db*' --exclude-table=$schemaname.cv_root_mview --exclude-table=$schemaname.db2cv_mview $sitedb > $file

pg_dump --user $dbuser --no-owner --inserts --blobs --table=$schemaname'.*_temp' --table=$schemaname'.*organism_stock_count*' --table=$schemaname'.*library_feature_count*' --table=$schemaname'.*organism_feature_count*' --table=$schemaname'.*analysis_organism*' --table=$schemaname'.*db2cv_mview*' --table=$schemaname'.*cv_root_mview*' $sitedb >> $file

# Remove SET statements
sed -i 's/^SET .*$//;/SELECT pg_catalog.*$/d' $file

#Since these constraints are not schema specific,
# we need to remove them to prevent conflicts.
# ALTER TABLE ONLY chado.tripal_gff_temp
#     ADD CONSTRAINT tripal_gff_temp__tripal_gff_temp_uq0__key UNIQUE (feature_id);
# ALTER TABLE ONLY chado.tripal_gff_temp
#     ADD CONSTRAINT tripal_gff_temp__tripal_gff_temp_uq1__key UNIQUE (uniquename, organism_id, type_name);
# ALTER TABLE ONLY chado.tripal_gffprotein_temp
#     ADD CONSTRAINT tripal_gffprotein_temp__tripal_gff_temp_uq0__key UNIQUE (feature_id);
# ALTER TABLE ONLY chado.tripal_obo_temp
#     ADD CONSTRAINT tripal_obo_temp__tripal_obo_temp0__key UNIQUE (id);
sed -i 's/ALTER TABLE ONLY.*//' $file
sed -i 's/ADD CONSTRAINT.*//' $file

# Switch schema qualification to chado in case that's not the schema set above
# to match with conventions.
sed -i "s/$schemaname\./chado./g" $file

# Remove all the comments and empty lines
sed -i 's/^\s*--.*$//;/^\s*$/d' $file
