/* Delete the improperly added foreign key from v1.3.3.002 */
ALTER TABLE db_relationship DROP CONSTRAINT db_relationship_type_id_fkey;
/* Add it back in with type_id => cvterm.cvterm_id as it should have */
ALTER TABLE db_relationship ADD CONSTRAINT db_relationship_type_id_fkey FOREIGN KEY  (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;
