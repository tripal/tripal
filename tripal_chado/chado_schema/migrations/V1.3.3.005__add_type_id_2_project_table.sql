/* https://github.com/GMOD/Chado/issues/37 */
ALTER TABLE chado.project
ADD COLUMN type_id bigint;
ALTER TABLE chado.project ADD FOREIGN KEY (type_id) REFERENCES chado.cvterm (cvterm_id) ON DELETE SET NULL;
CREATE INDEX project_idx1 ON chado.project USING btree (type_id);
COMMENT ON COLUMN chado.project.type_id IS 'An optional cvterm_id that specifies what type of project this record is.  Prior to 1.4, project type was set with an projectprop.';
