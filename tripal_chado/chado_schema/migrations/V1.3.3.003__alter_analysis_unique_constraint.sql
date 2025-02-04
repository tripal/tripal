ALTER TABLE analysis
  DROP CONSTRAINT analysis_c1
, ADD  CONSTRAINT analysis_c1 unique (program,programversion, name, sourcename);
