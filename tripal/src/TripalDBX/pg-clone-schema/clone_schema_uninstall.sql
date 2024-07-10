-- ===============================================
-- Uninstall PostgreSQL pg-clone-schema for Tripal
-- ===============================================
-- Any SQL code added here must be able to be run several times without side
-- effects. For instance, use "IF EXISTS" expressions when possible.

DROP FUNCTION IF EXISTS public.tripal_get_table_ddl(varchar, varchar, boolean);
DROP FUNCTION IF EXISTS public.tripal_clone_schema(text, text, boolean, boolean);
