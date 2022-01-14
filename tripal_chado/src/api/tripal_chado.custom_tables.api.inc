<?php
/**
 * @file
 * Provides an API to manage custom tables in Chado.
 */

/**
 * @defgroup tripal_custom_tables_api Chado Custom Tables
 * @ingroup tripal_chado_api
 * @{
 * Provides an application programming interface (API) for managing Chado
 *   tables. 
 *
 * @}
 */

 // Unofficial comments
// Edit an existing chado table
// It will fail if the table does not exist

/**
 * Edits a known custom table in the chado database. "Known" custom tables are
 * tables that exist in the tripal_custom_tables already. If it is not in tripal_custom_tables,
 * or the specified table does not exist, it will not attempt to make changes. 
 * It will also fail if the specified table is a materialized view.
 */

function chado_edit_custom_table($table_id, $table_name, $schema, $skip_if_exists = 1) {

  $messenger = \Drupal::messenger();
  $transaction = db_transaction();

    try {
        // Create a new record.
        $record = new stdClass();
        $record->table_id = $table_id;
        $record->table_name = $table_name;
        $record->schema = serialize($schema);

        // Get the current custom table record.
        $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = :table_id";
        $results = db_query($sql, [':table_id' => $table_id]);
        $custom_table = $results->fetchObject();

        // Check if the table is suitable for editing (not mview, doesn't exist in tripal_custom_tables table).
        if (!$custom_table) {
           // tripal_report_error('tripal_chado', TRIPAL_ERROR, "This table is not known to be a custom table. Please add it before trying to edit it.",[]);
            //$messenger->addError("This table is not known to be a custom table. Please use the " . `l('LINK TEXT', 'LINK_LOCATION') . " interface to add it.");
            return FALSE;
        }
        else if ($custom_table->mview_id) {
            tripal_report_error('tripal_chado', TRIPAL_ERROR, "Please use the chado_edit_mview() function to edit this custom table as it is a materialized view.", []);
            $messenger->addError("This custom table is a materialized view. Please use the " . l('Materialized View', 'admin/tripal/storage/chado/mviews') . " interface to edit it.");
            return FALSE;
        }

        // If the user changed the table name, we want to drop the old one and force
        // creation of the new one.
        if ($custom_table->table_name != $table_name) {
            chado_query("DROP TABLE %s", $custom_table->table_name);
            $skip_if_exists = 0; // we DO want to create the table now
        }

        // If skip creation is not set, then drop the table from chado if it exists.
        if (!$skip_if_exists) {
            // Check if the table exists with chado_table_exists instead of db_table_exists because
            //  that function is deprected in Drupal 8 and removed in Drupal 9
            if (chado_table_exists($custom_table->table_name)) {
                chado_query("DROP TABLE %s", $custom_table->table_name);
                $messenger->addStatus(t("Custom Table " . $custom_table->table_name . " dropped"));
            }
        }

        // Update the custom table record and run the create custom table function.
        \Drupal::database()->insert('tripal_custom_tables')
            ->key('table_id')
            ->fields(
                array(
                    'table_id' => $record->table_id,
                    'table_name' => $record->table_name,
                    'schema' => $record->schema
                    ))
            ->execute();
        $success = chado_create_custom_table($table_name, $schema, $skip_if_exists);

    }
    catch (Exception $e) {
        \Drupal::logger('tripal_chado')->error($e);
        $transaction->rollback();
        return FALSE;
    }
}

/**
 * Add a new table to the Chado schema.
 * 
 * This function is simply a wrapper for built-in database functionality
 * provided by Drupal, but ensures the table is created
 * inside the Chado schema rather than the Drupal schema.  
 * 
 * If the table already exists then it will be dropped and recreated using the
 * schema provided. However, it will only drop a table if it exsits in the
 * tripal_custom_tables table. This way the function cannot be used to
 * accidentally alter existing non custom tables.  If $skip_if_exists
 * is set then the table is simply added to the tripal_custom_tables
 * and no table is created in Chado.
 *
 * @param string $table
 * @param [type] $schema
 * @param boolean $skip_if_exists
 * @param [type] $mview_id
 * @param boolean $redirect
 * @return void
 */
function chado_create_custom_table($table, $schema, $skip_if_exists = TRUE, $mview_id = NULL, $redirect = TRUE) {
// Basic argument checks
    if (!$table)
    {
        tripal_report_error('trp_ctables', TRIPAL_ERROR,
            'Please provide a value for the $table argument to the chado_create_custom_table() function');
            return FALSE;
    }
    if (!$schema) {
        tripal_report_error('trp_ctables', TRIPAL_ERROR,
            'Please provide a value for the $schema argument to the chado_create_custom_table() function');
            return FALSE;
    }
    if ($schema and !is_array($schema)) {
        tripal_report_error('trp_ctables', TRIPAL_ERROR,
            'Please provide an array for the $schema argument to the chado_create_custom_table() function');
            return FALSE;
    }

    $created = 0;
    $recreated = 0;

    $chado_schema = chado_get_schema_name('chado'); // Todo - is this really how we want to get the schema name? 
                                                    // Does not seem like it would work with multi-chado instances.
    $chado_dot = $chado_schema . '.';
    $connection = \Drupal::database();
    $transaction = $connection->startTransaction();
    try {
        // See if the table already exists
        $table_exists = $connection->schema()->tableExists($chado_dot . $table);
        // See if the table is listed 
        $centry = $connection
                    ->select('tripal_custom_tables','tct')
                    ->fields('tct', array(
                        'table_name'
                    ))
                    ->condition('tct.table_name',$table)
                    ->execute()->fetchObject();
        
        // Create the table if it doesn't exist.
        if (!$table_exists) {
            $connection->schema()->createTable($chado_dot . $table, $schema);
            $created = 1;
        }

        // Recreate the table if it exists, if it's in the list, and if we aren't skipping existing ones
        if ($table_exists AND is_object($centry) AND !$skip_if_exists) {
            // Drop the table
            $connection->schema()->dropTable($chado_dot . $table);
            // And re-add it
            $connection->schema()->createTable($chado_dot . $table, $schema);
            $recreated = 1;

            // TODO the old code unset "referring_tables" from $schema,
            //      is this necessary?
        }

        // Add this table to tripal_custom_tables, dropping and re-adding if necessary
        if ($centry) {
            $connection->delete('tripal_custom_tables')
                        ->condition('table_name',$table)
                        ->execute();
        }
        // Assemble the record
        $record = array(
            'table_name' => $table,
            'schema'     => serialize($schema),
            'mview_id'   => $mview_id
        );
        $connection->insert('tripal_custom_tables')
            ->fields($record)
            ->execute();
                    
    } catch (Exception $e) {
        $transaction->rollback();

        \Drupal::logger('tripal_chado')->error($e);
    }
    
    return $created;
}

/**
 *  This function is used to validate a Drupal Schema API array prior to
 *  passing it ot the chado_create_custom_table_schema().  This function
 *  can be used in a form validate function or whenver a schema is provided by
 *  a user and needs validation.
 * 
 *  Current checks:
 *  - 'table' key exists
 *  - table name is lowercase
 *  - table name + index name       < 60 characters
 *  - table name + unique key name  < 60 characters
 *
 * @param array $schema_array
 *   An Drupal Schema API compatible array
 * 
 * @return string
 *   An empty string for success or a message string for failure
 */
function chado_validate_custom_table_schema($schema_array) {

    if (is_array($schema_array) and !array_key_exists('table', $schema_array)) {
        return "The schema array must have key named 'table'";
    }
    
    if (preg_match('/[ABCDEFGHIJKLMNOPQRSTUVWXYZ]/', $schema_array['table'])) {
        return "Postgres will automatically change the table name to lower-case. To prevent unwanted side-effects, please rename the table with all lower-case characters.";
    }

      // Check index length.
    if (array_key_exists('indexes', $schema_array)) {
        foreach ($schema_array['indexes'] as $index_name => $details) {
            if (strlen($schema_array['table'] . '_' . $index_name) > 60) {
                return "One ore more index names appear to be too long. For example: '" . $schema_array['table'] . '_' . $index_name .
                ".'  Index names are created by concatenating the table name with the index name provided " .
                "in the 'indexes' array of the schema. Please alter any indexes that when combined with the table name are " .
                "longer than 60 characters.";
            }
        }
    }
}

/**
 * Retrieve the custom table ID of the specified name
 *
 * @param string $table_name
 * @return int
 *   The unique identifier for the given table. FALSE if the
 *   table is not listed in the tripal_custom_tables table
 * 
 * @ingroup tripal_custom_tables_api
 */
function chado_get_custom_table_id($table_name) {
    try {
        $connection = \Drupal::database();
        
        $table_exists = $connection->schema()->tableExists('tripal_custom_tables');
        if ($table_exists) {
            $query = $connection->select('tripal_custom_tables','ct')
                ->fields('ct',array(
                    'table_id'
                ))
                ->condition('ct.table_name',$table_name)
                ->execute();
            $result = $query->fetchAll();
            if ($result) {
                return $result[0]->table_id;
            }
        }
        return FALSE;

    } catch (Exception $e) {
        tripal_set_message($e,TRIPAL_WARNING);
    }
}

/**
 * Retrieve a list of all Chado custom table names from the
 * tripal_custom_tables table.
 *
 * @param boolean $include_mview
 * @return array
 *  An associative array where the key and value pairs are the table name
 * 
 * @todo Make this more compact
 */
function chado_get_custom_table_names($include_mview = TRUE) {
    try {
        $connection = \Drupal::database();

        if ($include_mview) {
            $query = $connection->select('tripal_custom_tables','ct')
            ->fields('ct', array(
                'table_name'
            ))
            ->execute();
        }
        else {
            $query = $connection->select('tripal_custom_tables','ct')
            ->fields('ct', array(
                'table_name'
            ))
            ->isNull('ct.mview_id')
            ->execute();
        }

        $results = $query->fetchAll();
        foreach($results as $result) {
            $tables[$result->table_name] = $result->table_name;
        }
        if($tables) {asort($tables);}
        return $tables;

    } catch (Exception $e) {
        tripal_set_message($e,TRIPAL_WARNING);
    }
}

/**
 * Deletes the specified table
 *
 * @param [type] $table_id
 * @return void
 */
function chado_delete_custom_table($table_id) {
    $connection = \Drupal::database();
    $transaction = $connection->startTransaction();

    try {
        // Get details about this table from tripal_custom_tables
        $query = $connection->select('tripal_custom_tables', 'ct')
            ->fields('ct', array(
                'table_id',
                'table_name',
                'schema',
                'mview_id'
            ))
            ->condition('ct.table_id',$table_id)
            ->execute();
        $table_details = $query->fetchObject();
        //print_r($table_details);
        
        // Don't delete if this table is a materialized view 
        if($table_details->mview_id) {
            tripal_report_error('tripal_chado', TRIPAL_ERROR, "Please use the chado_delete_mview() function to delete this custom table as it is a materialized view. Table not deleted.", []);
            //$messenger->addError("This custom table is a materialized view. Please use the " . l('Materialized View', 'admin/tripal/storage/chado/mviews') . " interface to delete it.");
            //return FALSE;
        }

        // Also check if it exists in the database as promised.
        $chado_schema = chado_get_schema_name('chado'); // Todo - is this really how we want to get the schema name? 
                                                    // Does not seem like it would work with multi-chado instances.
        $chado_dot = $chado_schema . '.';
        $table_exists = $connection->schema()->tableExists($chado_dot . $table_details->table_name);

        // Return errors if the table doesn't exist (either in
        // tripal_custom_tables or in the database itself).
        if (!$table_details OR !$table_exists) {
            $message = "The table " . $table_details->table_name . " is either not listed in the tripal_custom_tables table or it doesn't exist in the database.\n";
            \Drupal::logger('tripal_chado')->notice($message);
            return FALSE;
        }

        // Remove the entry from the tripal_custom_tables table.
        $delisted = $connection->delete('tripal_custom_tables')
            ->condition('table_id',$table_id)
            ->execute();
        // Drop the table from Chado (checking if it exists)
        $deleted = $connection->schema()->dropTable($chado_dot . $table_details->table_name);
        if ($delisted AND $deleted) {
            return TRUE;
        }
        else {
            \Drupal::logger('tripal_chado')->error('Could not delete the ". $table_details->table_name ." table');
        }
        
    } catch (Exception $e) {
        tripal_set_message($e,TRIPAL_WARNING);
    }
}