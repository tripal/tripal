/* https://github.com/GMOD/Chado/issues/70 */
CREATE INDEX cvtermsynonym_idx2 ON cvtermsynonym (type_id);
CREATE INDEX cvtermsynonym_idx3 ON cvtermsynonym (synonym);
