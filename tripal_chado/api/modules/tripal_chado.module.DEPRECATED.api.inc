<?php

/**
 * @file
 *
 * These api functions are deprecated, if your site is currently using them
 * please update your code with the newer tripal_chado functions.
 */

/**
 * @defgroup tripal_chado_module_DEPRECATED_api
 * @ingroup tripal_chado_api
 * @{
 * Deprecated legacy api code.
 * @}
 */


/**tripal_analysis_api*/
/**
 * Retrieves a chado analysis variable.
 *
 * @param $identifier
 *   an array with the key stating what the identifier is. Supported keys
 *   (only on of the following unique keys is required):
 *    - analysis_id: the chado analysis.analysis_id primary key.
 *    - nid: the drupal node.nid primary key.
 *   There are also some specially handled keys. They are:
 *    - property: An array/object describing the property to select records for.
 *      It should at least have either a type_name (if unique across cvs) or
 *      type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the stock record
 * to be returned.
 *
 * @return
 *   the analysis node matching the passed in identifier
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_analysis($identifier, $options) {
  return chado_get_analysis($identifier, $options);
}

/**
 * Returns a list of analyses that are currently synced with Drupal to use in
 * select lists.
 *
 * @param $syncd_only
 *   Whether or not to return all chado analyses or just those sync'd with
 *   drupal. Defaults to TRUE (only sync'd analyses).
 *
 * @return
 *   An array of analyses sync'd with Drupal where each value is the analysis
 *   scientific name and the keys are analysis_id's.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_analysis_select_options($syncd_only = TRUE) {
  return chado_get_analysis_select_options($syncd_only);
}

/**tripal_contact_api*/
/**
 * Adds a contact to the Chado contact table.
 *
 * @param $values
 *   An array of values to be inserted. Valid keys include:
 *   - name: The name of the contact.
 *   - description: Text describing the contact.
 *   - type_name: The type of contact.  Must be a term in the tripal_contact
 *     vocabulary.
 *   - properties: An associative array containing a list of key value pairs for
 *     the properites. The key's must be valid terms in the tripal_contact
 *     vocabulary (e.g. Affiliation, Address, etc).
 *
 * @return
 *   On success, an array is returned containing the fields of the contact
 *   record including the newly added contact_id. On failure, FALSE is
 *   returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_insert_contact($values) {
  return chado_insert_contact($values);
}

/**
 * This function is intended to be used in autocomplete forms for contacts.
 *
 * @param $text
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_autocomplete_contact($text) {
  return chado_autocomplete_contact($text);
}

/**tripal_chado_cv_api*/
/**
 * Retrieves a chado controlled vocabulary variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only
 *   on of the following unique keys is required):
 *    - cv_id: the chado cv.cv_id primary key.
 *    - name: the chado cv.name field (assume unique).
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition fot additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the cv record to
 * be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the cv will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_cv($identifiers, $options = []) {
  return chado_get_cv($identifiers, $options);
}

/**
 * Create an options array to be used in a form element which provides a
 * list of all chado cvs.
 *
 * @return
 *   An array(cv_id => name) for each cv in the chado cv table.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_cv_select_options() {
  return chado_get_cv_select_options();
}

/**
 * Retrieves a chado controlled vocabulary term variable.
 *
 * @param $identifier
 *   An array apropriate for use with the chado_generate_var for uniquely
 *   identifying a cvterm record. Alternativley, there are also some specially
 *   handled keys. They are:
 *    - id: an ID for the term of the for [dbname]:[accession], where [dbname]
 *      is the short name of the vocabulary and accession is the unique ID.
 *    - cv_id:  an integer indicating the cv_id or an array with 'name' => the
 *      name of the cv.
 *    - synonym: an array with 'name' => the name of the synonym of the cvterm
 *      you want returned; 'cv_id' => the cv_id of the synonym; 'cv_name' =>
 *      the name of the cv of the synonym.
 *    - property: An array/object describing the property to select records
 *      for. It should at least have either a type_name (if unique across cvs)
 *      or type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 *   passed into chado_select_record(). It should fully specify the cvterm
 *   record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the cvterm will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_cvterm($identifiers, $options = []) {
  return chado_get_cvterm($identifiers, $options);
}

/**
 * Create an options array to be used in a form element
 *   which provides a list of all chado cvterms.
 *
 * @param $cv_id
 *   The chado cv_id; only cvterms with the supplied cv_id will be returnedl.
 * @param $rel_type
 *   Set to TRUE if the terms returned should only be relationship types in
 *   the vocabulary.  This is useful for creating drop-downs of terms
 *   used for relationship linker tables.
 *
 * @return
 *   An associative array with the cvterm_id's as keys. The first
 *   element in the array has a key of '0' and a value of 'Select a Type'.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_cvterm_select_options($cv_id, $rel_type = FALSE) {
  return chado_get_cvterm_select_options($cv_id, $rel_type);
}

/**
 * Duplicate of fill_cvtermpath() stored procedure in Chado.
 *
 * Identifies all of the root terms of the controlled vocabulary.
 *
 * @param $cvid
 *   The controlled vocabulary ID from the cv table of Chado (i.e. cv.cv_id).
 * @param $job_id
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_update_cvtermpath($cv_id, $job_id = NULL) {
  return chado_update_cvtermpath($cv_id, $job_id);
}

/**
 * Adds a controlled vocabulary to the CV table of Chado.
 *
 * @param $name
 *   The name of the controlled vocabulary. These are typically all lower case
 *   with no special characters other than an undrescore (for spaces).
 * @param $comment
 *   A description or definition of the vocabulary.
 *
 * @return
 *   An object populated with fields from the newly added database.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_insert_cv($name, $definition) {
  return chado_insert_cv($name, $definition);
}

/**
 *  Add's a controlled vocabulary term to Chado.
 *
 *  This function will add a cvterm record (and a dbxref record if appropriate
 *  values are provided). If the parent vocabulary does not exist then
 *  that also is added to the cv table.  If the cvterm is a relationship term
 *  then the 'is_relationship' value should be set.  All
 *  terms must also have a corresponding database.  This is specified in the
 *  term's ID just before the colon (e.g. GO:003824).  If the database does not
 *  exist in the DB table then it will be added automatically.  The accession
 *  (the value just after the colon in the term's ID) will be added to the
 *  dbxref table.  If the CVterm already exists and $update is set (default)
 *  then the cvterm is updated.  If the CVTerm already exists and $update is
 *  not set, then no changes are made and the CVTerm object is returned.
 *
 * @param $term
 *   An associative array with the following keys:
 *    - id: the term accession. must be of the form <DB>:<ACCESSION>, where
 *      <DB> is the name of the database to which the cvterm belongs and the
 *      <ACCESSION> is the term's accession number in the database.
 *    - name: the name of the term. usually meant to be human-readable.
 *    - is_obsolete: is present and set to 1 if the term is defunct.
 *    - definition: the definition of the term.
 *    - cv_name: The CV name to which the term belongs.  If this arugment is
 *        null or not provided then the function tries to find a record in the
 *        CV table with the same name provided in the $term[namespace].  If
 *        this field is provided then it overrides what the value in
 *        $term[namespace].
 *    - is_relationship: If this term is a relationship term then this value
 *        should be 1.
 *    - db_name: In some cases the database name will not be part of the
 *        $term['id'] and it needs to be explicitly set.  Use this argument
 *        only if the database name cannot be specififed in the term ID
 *        (e.g. <DB>:<ACCESSION>).
 * @param $options
 *   An associative array with the following keys:
 *    - update_existing: By default this is TRUE.  If the term exists it is
 *      automatically updated.
 *
 * @return
 *   A cvterm object
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_insert_cvterm($term, $options = []) {
  return chado_insert_cvterm($term, $options);
}

/**
 * TODO: deprecate this function
 *
 * Avoid using this function as it will be deprecated in future releases.
 *
 * This function allows other modules to programatically
 * submit an ontology for loading into Chado.
 *
 * This function will add a job to the Jobs subsystem for parsing the ontology.
 * You can either pass a known OBO ID to the function or the URL
 * or full path the the ontology file.  If a URL or file name is
 * passed then the $obo_name argument must also be provided.  If
 * this is the first time the ontology has been provided to Tripal
 * then it will be added to the database and will be assigned a
 * unique OBO ID.
 *
 * @param $obo_id
 *   If the ontology is already loaded into the Tripal tables then
 *   use this argument to specify the unique ID for the ontology
 *   that will be loaded.
 * @param $obo_name
 *   If the OBO has not been added before then use this argument
 *   to specify the human readable name of the ontology.
 * @param $obo_url
 *   If the OBO to be loaded is located on a remote server then
 *   use this argument to provide the URL.
 * @param $obo_file
 *   If the OBO is housed on the local file system of the server then
 *   use this argument to specify the full path.
 *
 * @return
 *   returns the job_id of the submitted job or FALSE if the job was not added
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_submit_obo_job($obo) {
  return chado_submit_obo_job($obo);
}

/**
 * Add the OBO to the tripal_cv_obo table in the Drupal database.
 *
 * If the OBO name already exists in the table then the path is updated.
 *
 * @param $name
 *   The human readable name of this ontology.
 * @param $path
 *   The file path or URL of the ontology.
 *
 * @return
 *   Returns the ontology ID.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_insert_obo($name, $path) {
  return chado_insert_obo($name, $path);
}

/**
 * Retrieves an OBO record.
 *
 * @param $values
 *   An associate array with the following allowed keys: obo_id, name.
 *
 * @return
 *   An instance of an OBO record object.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_obo($values) {
  return chado_get_obo($values);
}

/**
 * This function is intended to be used in autocomplete forms.
 *
 * This function searches for a matching controlled vobulary name.
 *
 * @param $string
 * The string to search for.
 *
 * @return
 * A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_autocomplete_cv($string = '') {
  return chado_autocomplete_cv($string);
}

/**
 * This function is intended to be used in autocomplete forms
 * for searching for CV terms that begin with the provided string.
 *
 * @param $cv_id
 * The CV ID in which to search for the term.
 * @param $string
 * The string to search for.
 *
 * @return
 * A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_autocomplete_cvterm($cv_id, $string = '') {
  return chado_autocomplete_cvterm($cv_id, $string);
}

/**
 * Add a record to a cvterm linking table (ie: feature_cvterm).
 *
 * @param $basetable
 *   The base table to which the cvterm should be linked/associated. Thus to
 *   associate a cvterm to a feature the basetable=feature and cvterm_id is
 *   added to the feature_cvterm table.
 * @param $record_id
 *   The primary key of the basetable to associate the cvterm with. This should
 *   be in integer.
 * @param $cvterm
 *   An associative array describing the cvterm. Valid keys include:
 *     - name: the name for the cvterm,
 *     - cv_name: the name of the cv the cvterm belongs to.
 *     - cv_id: the primary key of the cv the cvterm belongs to.
 * @param $options
 *   An associative array of options. Valid keys include:
 *     - insert_cvterm: Insert the cvterm if it doesn't already exist. FALSE is
 *       the default.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_associate_cvterm($basetable, $record_id, $cvterm, $options = []) {
  return chado_associate_cvterm($basetable, $record_id, $cvterm, $options);
}

/**tripal_chado_database_api*/
/**
 * Retrieves a chado db variable.
 *
 * Example Usage:
 *
 * @code
 *   $select_values = array(
 *     'name' => 'SOFP'
 *   );
 *   $db_object = tripal_get_db($select_values);
 * @endcode
 *  The above code selects the SOFP db and returns the following object:
 * @code
 *   $db_object = stdClass Object (
 *     [db_id] => 49
 *     [name] => SOFP
 *     [description] =>
 *     [urlprefix] =>
 *     [url] =>
 *   );
 * @endcode
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only
 *   on of the following unique keys is required):
 *    - db_id: the chado db.db_id primary key.
 *    - name: the chado db.name field (assume unique).
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the db record to
 * be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the cv will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, an array of objects will be returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_db($identifiers, $options = []) {
  return chado_get_db($identifiers, $options);
}

/**
 * Create an options array to be used in a form element
 *   which provides a list of all chado dbs.
 *
 * @return
 *   An array(db_id => name) for each db in the chado db table.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_db_select_options() {
  return chado_get_db_select_options();
}

/**
 * Retrieves a chado database reference variable.
 *
 * Example Usage:
 *
 * @code
 *   $identifiers = array(
 *     'accession' => 'synonym',
 *     'db_id' => array(
 *       'name' => 'SOFP'
 *     )
 *   );
 *   $dbxref_object = tripal_get_dbxref($identifiers);
 * @endcode
 *  The above code selects the synonym database reference and returns the
 *  following object:
 * @code
 *  $dbxref_object = stdClass Object (
 *     [dbxref_id] => 2581
 *     [accession] => synonym
 *     [description] =>
 *     [version] =>
 *     [db_db_id] => 49
 *     [db_name] => SOFP
 *     [db_description] =>
 *     [db_urlprefix] =>
 *     [db_url] =>
 *   );
 * @endcode
 *
 * @param $identifier
 *   An array apropriate for use with the chado_generate_var for uniquely
 *   identifying a dbxref record. Alternatively, there are also some specially
 *   handled keys. They are:
 *    - property: An array/object describing the property to select records for.
 *      It should at least have either a type_name (if unique across cvs) or
 *      type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the dbxref record
 * to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the dbxref will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_dbxref($identifiers, $options = []) {
  return chado_get_dbxref($identifiers, $options);
}

/**
 * Generates a URL for the controlled vocabulary term.
 *
 * If the URL and URL prefix are provided for the database record of a cvterm
 * then a URL can be created for the term.  By default, the db.name and
 * dbxref.accession are concatenated and appended to the end of the
 * db.urlprefix. But Tripal supports the use of {db} and {accession} tokens
 * when if present in the db.urlprefix string will be replaced with the db.name
 * and dbxref.accession respectively.
 *
 * @param $dbxref
 *   A dbxref object as created by the chado_generate_var() function.
 *
 * @return
 *   A string containing the URL.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_dbxref_url($dbxref) {
  return chado_get_dbxref_url($dbxref);
}

/**
 * Adds a new database to the Chado DB table and returns the DB object.
 *
 * @param $values
 *   An associative array of the values of the db (those to be inserted):
 *   - name: The name of the database. This name is usually used as the prefix
 *     for CV term accessions.
 *   - description: (Optional) A description of the database.  By default no
 *     description is required.
 *   - url: (Optional) The URL for the database.
 *   - urlprefix: (Optional) The URL that is to be used as a prefix when
 *     constructing a link to a database term.
 * @param $options
 *   Optional. An associative array of options that can include:
 *   - update_existing: Set this to '1' to force an update of the database if it
 *     already exists. The default is to not update. If the database exists
 *     then nothing is added.
 *
 * @return
 *   An object populated with fields from the newly added database.  If the
 *   database already exists it returns the values in the current entry.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_insert_db($values, $options = []) {
  return chado_insert_db($values, $options);
}

/**
 * Add a database reference.
 *
 * @param $values
 *   An associative array of the values to be inserted including:
 *    - db_id: the database_id of the database the reference is from.
 *    - accession: the accession.
 *    - version: (Optional) The version of the database reference.
 *    - description: (Optional) A description of the database reference.
 *
 * @return
 *   The newly inserted dbxref as an object, similar to that returned by
 *   the chado_select_record() function.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_insert_dbxref($values) {
  return chado_insert_dbxref($values);
}

/**
 * Add a record to a database reference linking table (ie: feature_dbxref).
 *
 * @param $basetable
 *   The base table for which the dbxref should be associated. Thus to associate
 *   a dbxref with a feature the basetable=feature and dbxref_id is added to the
 *   feature_dbxref table.
 * @param $record_id
 *   The primary key of the basetable to associate the dbxref with. This should
 *   be in integer.
 * @param $dbxref
 *   An associative array describing the dbxref. Valid keys include:
 *   'accession' => the accession for the dbxref, 'db_name' => the name of the
 *    database the dbxref belongs to.
 *   'db_id' => the primary key of the database the dbxref belongs to.
 * @param $options
 *   An associative array of options. Valid keys include:
 *    - insert_dbxref: Insert the dbxref if it doesn't already exist. TRUE is
 *      the default.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_associate_dbxref($basetable, $record_id, $dbxref, $options = []) {
  return chado_associate_dbxref($basetable, $record_id, $dbxref, $options);
}

/**
 * This function is intended to be used in autocomplete forms
 * for searching for accession that begin with the provided string.
 *
 * @param $db_id
 *   The DB ID in which to search for the term.
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_autocomplete_dbxref($db_id, $string = '') {
  return chado_autocomplete_dbxref($db_id, $string);
}

/**tripal_feature_api*/
/**
 * Used for autocomplete in forms for identifying for publications.
 *
 * @param $field
 *   The field in the publication to search on.
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_autocomplete_feature($string = '') {
  return chado_autocomplete_feature($string);
}

/**
 * Performs a reverse compliment of a nucleotide sequence.
 *
 * @param $sequence
 *   The nucelotide sequence.
 *
 * @return
 *   an upper-case reverse complemented sequence.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_reverse_compliment_sequence($sequence) {
  return chado_reverse_compliment_sequence($sequence);
}

/**
 * Retrieves the sequences for a given feature.
 *
 * If a feature has multiple alignments or multiple relationships then
 * multiple sequences will be returned.
 *
 * @param $feature
 *   An associative array describing the feature. Valid keys include:
 *    - feature_id: The feature_id of the feature for which the sequence will
 *      be retrieved.
 *    - name: The feature name. This will appear on the FASTA definition line.
 *    - parent_id:  (optional) only retrieve a sequence if 'derive_from_parent'
 *      is true and the parent matches this ID.
 *    - featureloc_id: (optional) only retrieve a sequence if
 *      'derive_from_parent' is true and the alignment is defined with this
 *      featureloc_id.
 * @param $options
 *   An associative array of options. Valid keys include:
 *    - width: Indicate the number of bases to use per line.  A new line will
 *      be added after the specified number of bases on each line.
 *    - is_html: Set to '1' if  the sequence is meant to be displayed on a web
 *      page. This will cause a <br> tag to separate lines of the FASTA
 *   sequence.
 *    - derive_from_parent: Set to '1' if the sequence should be obtained from
 *      the parent to which this feature is aligned.
 *    - aggregate: Set to '1' if the sequence should only contain sub features,
 *      excluding intro sub feature sequence.  For example, set this option to
 *      obtain just the coding sequence of an mRNA.
 *    - upstream: An integer specifing the number of upstream bases to include
 *      in the output.
 *    - downstream: An integer specifying the number of downstream bases to
 *      include in the output.
 *    - sub_feature_types: Only include sub features (or child features) of
 *      the types provided in the array.
 *    - relationship_type: If a relationship name is provided (e.g.
 *   sequence_of)
 *      then any sequences that are in relationships of this type with matched
 *      sequences are also included.
 *    - relationship_part: If a relationship is provided in the preceding
 *      argument then the rel_part must be either 'object' or 'subject' to
 *      indicate which side of the relationship the matched features belong.
 *
 * @return
 *   an array of matching sequence in the following keys for each sequence:
 *    - types: an array of feature types that were used to derive
 *      the sequence (e.g. from an aggregated sequence)
 *    - upstream: the number of upstream bases included in the sequence
 *    - downstream: the number of downstream bases included in the
 *      sequence
 *    - defline: the definintion line used to create a FASTA sequence
 *    - residues: the residues
 *    - featureloc_id: the featureloc_id if the sequences is from an
 *        alignment
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_feature_sequences($feature, $options) {
  return chado_get_feature_sequences($feature, $options);
}

/**
 * Retrieves the bulk sequences for a given feature.
 *
 * @param $options
 *   An associative array of options for selecting a feature. Valid keys
 *   include:
 *    - org_commonname: The common name of the organism for which sequences
 *      should be retrieved
 *    - genus: The genus of the organism for which sequences should be
 *   retrieved
 *    - species: The species of the organism for which sequences should be
 *      retrieved
 *    - analysis_name: The name of an analysis to which sequences belong. Only
 *      those that are associated with the analysis will be retrieved.
 *    - type: The type of feature (a sequence ontology term).
 *    - feature_name: the name of the feature. Can be an array of feature
 *   names.
 *    - feature_uname: the uniquename of the feature.  Can be an array of
 *      feature unique names.
 *    - upstream: An integer specifing the number of upstream bases to include
 *      in the output
 *    - downstream: An integer specifying the number of downstream bases to
 *      include in the output.
 *    - derive_from_parent: Set to '1' if the sequence should be obtained from
 *      the parent to which this feature is aligned.
 *    - aggregate: Set to '1' if the sequence should only contain sub features,
 *      excluding intro sub feature sequence.  For example, set this option to
 *      obtain just the coding sequence of an mRNA.
 *    - sub_feature_types: Only include sub features (or child features) of
 *      the types provided in the array
 *    - relationship_type: If a relationship name is provided (e.g.
 *   sequence_of)
 *      then any sequences that are in relationships of this type with matched
 *      sequences are also included
 *    - relationship_part: If a relationship is provided in the preceding
 *      argument then the rel_part must be either 'object' or 'subject' to
 *      indicate which side of the relationship the matched features belong
 *    - width: Indicate the number of bases to use per line.  A new line will
 *      be added after the specified number of bases on each line.
 *    - is_html: Set to '1' if  the sequence is meant to be displayed on a
 *      web page. This will cause a <br> tag to separate lines of the FASTA
 *      sequence.
 *
 * @return
 *   Returns an array of sequences. The sequences will be in an array with the
 *   following keys for each sequence:
 *      'types'         => an array of feature types that were used to derive
 *         the sequence (e.g. from an aggregated sequence)
 *      'upstream'      => the number of upstream bases in the sequence
 *      'downstream'    => the number of downstream bases in the sequence
 *      'defline'       => the definintion line used to create a FASTA sequence
 *      'residues'      => the residues
 *      'featureloc_id' => the featureloc_id if from an alignment
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_bulk_feature_sequences($options) {
  return chado_get_bulk_feature_sequences($options);
}

/**
 * Returns a definition line that can be used in a FASTA file.
 *
 * @param $feature
 *   A single feature object containing all the fields from the chado.feature
 *   table. Best case is to provide an object generated by the
 *   chado_generate_var() function.
 * @param $notes
 *   Optional: additional notes to be added to the definition line.
 * @param $featureloc
 *   Optional: a single featureloc object generated using chado_generate_var
 *   that contains a record from the chado.featureloc table. Provide this if the
 *   sequence was obtained by using the alignment rather than from the
 *   feature.residues column.
 * @param $type
 *   Optional: the type of sequence. By default the feature type is used.
 * @param $length
 *   Optional: the length of the sequence.
 *
 * @return
 *   A string of the format: uniquename|name|type|feature_id
 *   or if an alignment:  srcfeature_name:fmin..fmax[+-]; alignment of
 *   uniquename|name|type|feature_id.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_fasta_defline($feature, $notes = '', $featureloc = NULL, $type = '', $length = 0) {
  return chado_get_fasta_defline($feature, $notes, $featureloc, $type, $length);
}

/**
 * Returns a string representing a feature location in an alignment.
 *
 * @param $featureloc
 *   A single featureloc object generated using chado_generate_var that
 *   contains a record from the chado.featureloc table.
 *
 * @return
 *   A string of the format: uniquename:featurelocmin..featurelocmax.strand
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_location_string($featureloc) {
  return chado_get_location_string($featureloc);
}

/**
 * Retrieves a chado organism variable.
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only
 *   on of the following unique keys is required):
 *    - organism_id: the chado organism.organism_id primary key.
 *    - genus & species: the chado organism.genus field & organism.species
 *   field. There are also some specially handled keys. They are:
 *    - property: An array/object describing the property to select records
 *   for.
 *      It should at least have either a type_name (if unique across cvs) or
 *      type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *      definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the organism
 * record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the organism will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_organism($identifiers, $options = []) {
  return chado_get_organism($identifiers, $options);
}

/**
 * Returns the full scientific name of an organism.
 *
 * @param $organism
 *   An organism object.
 *
 * @return
 *   The full scientific name of the organism.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_organism_scientific_name($organism) {
  return chado_get_organism_scientific_name($organism);
}

/**
 * Returns a list of organisms that are currently synced with Drupal to use in
 * select lists.
 *
 * @param $syncd_only
 *   Whether or not to return all chado organisms or just those sync'd with
 *   drupal. Defaults to TRUE (only sync'd organisms).
 *
 * @return
 *   An array of organisms sync'd with Drupal where each value is the organism
 *   scientific name and the keys are organism_id's.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_organism_select_options($syncd_only = TRUE) {
  return chado_get_organism_select_options($syncd_only);
}

/**
 * Return the path for the organism image.
 *
 * @param $organism
 *   An organism table record.
 *
 * @return
 *   If the type parameter is 'url' (the default) then the fully qualified
 *   url to the image is returend. If no image is present then NULL is returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_organism_image_url($organism) {
  return chado_get_organism_image_url($organism);
}

/**
 * This function is intended to be used in autocomplete forms
 * for searching for organisms that begin with the provided string.
 *
 * @param $text
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_autocomplete_organism($text) {
  return chado_autocomplete_organism($text);
}

/**
 * A handy function to abbreviate the infraspecific rank.
 *
 * @param $rank
 *   The rank below species.
 *
 * @return
 *   The proper abbreviation for the rank.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_abbreviate_infraspecific_rank($rank) {
  return chado_abbreviate_infraspecific_rank($rank);
}

/** tripal_phylotree_api */
/**
 * Validates an $options array for insert or update of a phylotree record.
 *
 * If validation passes then any values that needed validation lookups
 * (such as the dbxref, analysis, leaf_type, etc) will have their approriate
 * primary_keys added to the $options array, and missing default values
 * will also be added.
 *
 * @param $val_type
 *   The type of validation. Can be either 'insert' or 'update'.
 * @param $options
 *   An array of key/value pairs containing any of the valid keys for
 *   either the tripal_insert_phylotree() or tripal_update_phylotree()
 *   functions.
 * @param $errors
 *   An empty array where validation error messages will be set. The keys
 *   of the array will be name of the field from the options array and the
 *   value is the error message.
 * @param $warnings
 *   An empty array where validation warning messagges will be set. The
 *   warnings should not stop an insert or an update but should be provided
 *   to the user as information by a drupal_set_message() if appropriate. The
 *   keys of the array will be name of the field from the options array and the
 *   value is the error message.
 *
 * @return
 *   If validation fails then FALSE is returned.  Any options that do not pass
 *   validation checks will be added in the $errors array with the key being
 *   the option and the value being the error message.  If validation
 *   is successful then TRUE is returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_validate_phylotree($val_type, &$options, &$errors, &$warnings) {
  return chado_validate_phylotree($val_type, $options, $errors, $warnings);
}

/**
 * Inserts a phylotree record into Chado.
 *
 * This function validates the options passed prior to insertion of the record,
 * and if validation passes then any values in the options array that needed
 * validation lookups (such as the dbxref, analysis, leaf_type, etc) will have
 * their approriate primary key values added to the options array.
 *
 * @param $options
 *  An array of key value pairs with the following keys required:
 *     'name':       The name of the tree. This will be displayed to users.
 *     'description: A description about the tree
 *     'anlaysis_id: The ID of the analysis to which this phylotree should be
 *                   associated.
 *     'analysis':   If the analysis_id key is not used then the analysis name
 *                   may be provided to identify the analysis to which the tree
 *                   should be associated.
 *     'leaf_type':  A sequence ontology term or the word 'organism'. If the
 *                   type is 'organism' then this tree represents a
 *                   taxonomic tree.  The default, if not specified, is the
 *                   term 'polypeptide'.
 *     'tree_file':  The path of the file containing the phylogenetic tree to
 *                   import or a Drupal managed_file numeric ID.
 *     'format':     The file format. Currently only 'newick is supported'.
 *
 *  Optional keys:
 *     'dbxref':     A database cross-reference of the form DB:ACCESSION.
 *                   Where DB is the database name, which is already present
 *                   in Chado, and ACCESSION is the unique identifier for
 *                   this tree in the remote database.
 *     'name_re':    If the leaf type is NOT 'taxonomy', then the value of
 *                   this field can be a regular expression to pull out
 *                   the name of the feature from the node label in the
 *                   intput tree. If no value is provided the entire label is
 *                   used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *     'load_now':   If set, the tree will be loaded immediately if a tree_file
 *                   is provided. Otherwise, the tree will be loaded via
 *                   a Tripal jobs call.
 *     'no_load':    If set the tree file will not be loaded.
 * @param $errors
 *   An empty array where validation error messages will be set. The keys
 *   of the array will be name of the field from the options array and the
 *   value is the error message.
 * @param $warnings
 *   An empty array where validation warning messagges will be set. The
 *   warnings should not stop an insert or an update but should be provided
 *   to the user as information by a drupal_set_message() if appropriate. The
 *   keys of the array will be name of the field from the options array and the
 *   value is the error message.
 *
 * @return
 *   TRUE for success and FALSE for failure.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_insert_phylotree(&$options, &$errors, &$warnings) {
  return chado_insert_phylotree($options, $errors, $warnings);
}

/**
 * Updates a phylotree record into Chado.
 *
 * This function validates the options passed prior to update of the record
 * and if validation passes then any values in the options array that needed
 * validation lookups (such as the dbxref, analysis, leaf_type, etc) will have
 * their approriate primary key values added to the options array. A Drupal
 * File object will be added to the options array for the tree file if one
 * is provided.
 *
 *
 * @param $phylotree_id
 *   The ID of the phylotree to update.
 * @param $options
 *  An array of key value pairs with the following optional keys:
 *     'name':       The name of the tree. This will be displayed to users.
 *     'description: A description about the tree
 *     'anlaysis_id: The ID of the analysis to which this phylotree should be
 *                   associated.
 *     'analysis':   If the analysis_id key is not used then the analysis name
 *                   may be provided to identify the analysis to which the tree
 *                   should be associated.
 *     'leaf_type':  A sequence ontology term or the word 'organism'. If the
 *                   type is 'organism' then this tree represents a
 *                   taxonomic tree.  The default, if not specified, is the
 *                   term 'polypeptide'.
 *     'tree_file':  The path of the file containing the phylogenetic tree to
 *                   import or a Drupal managed_file numeric ID.
 *     'format':     The file format. Currently only 'newick is supported'
 *     'dbxref':     A database cross-reference of the form DB:ACCESSION.
 *                   Where DB is the database name, which is already present
 *                   in Chado, and ACCESSION is the unique identifier for
 *                   this tree in the remote database.
 *     'name_re':    If the leaf type is NOT 'taxonomy', then the value of
 *                   this field can be a regular expression to pull out
 *                   the name of the feature from the node label in the
 *                   intput tree. If no value is provided the entire label is
 *                   used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *     'load_now':   If set, the tree will be loaded immediately if a tree_file
 *                   is provided. Otherwise, the tree will be loaded via
 *                   a Tripal jobs call.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_update_phylotree($phylotree_id, &$options) {
  return chado_update_phylotree($phylotree_id, $options);
}

/**
 * Deletes a phylotree record from Chado.
 *
 * @param $phylotree_id
 *
 * @return
 *   TRUE on success, FALSE on failure.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_delete_phylotree($phylotree_id) {
  return chado_delete_phylotree($phylotree_id);
}

/**
 * Iterates through the tree and sets the left and right indicies.
 *
 * @param $tree
 *   The tree array.
 * @param $index
 *   This parameters is not used when the function is first called. It
 *   is used for recursive calls.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_assign_phylogeny_tree_indices(&$tree, &$index = 1) {
  return chado_assign_phylogeny_tree_indices($tree, $index);
}

/**
 * Iterates through the tree array and creates phylonodes in Chado.
 *
 * The function iterates through the tree in a top-down approach adding
 * parent internal nodes prior to leaf nodes.  Each node of the tree should have
 * the following fields:
 *
 *   -name:         The name (or label) for this node.
 *   -depth:        The depth of the node in the tree.
 *   -is_root:      Set to 1 if this node is a root node.
 *   -is_leaf:      Set to 1 if this node is a leaf node.
 *   -is_internal:  Set to 1 if this node is an internal node.
 *   -left_index:   The index of the node to the left in the tree.
 *   -right_index:  The index of the node to the right in the tree.
 *   -branch_set:   An array containing a list of nodes of that are children
 *                  of the node.
 *   -parent:       The name of the parent node.
 *   -organism_id:  The organism_id for associtating the node with an organism.
 *   -properties:   An array of key/value pairs where the key is the cvterm_id
 *                  and the value is the property value.  These properties
 *                  will be associated with the phylonode.
 *
 * Prior to importing the tree the indicies can be set by using the
 * chado_assign_phylogeny_tree_indices() function.
 *
 * @param $tree
 *   The tree array.
 * @param $phylotree .
 *   The phylotree object (from Chado).
 * @param $options
 *   The options provide some direction for how the tree is imported.  The
 *   following keys can be used:
 *   -taxonomy:  Set to 1 if this tree is a taxonomic tree. Set to 0
 *               otherwise.
 *   -leaf_type: Set to the leaf type name. If this is a non-taxonomic tree
 *               that is associated with features, then this should be the
 *               Sequence Ontology term for the feature (e.g. polypeptide).
 *               If this is a taxonomic tree then this option is not needed.
 *   -match:     Set to either 'name' or 'uniquename'.  This is used for
 *               matching the feature name or uniquename with the node name.
 *               This is not needed for taxonomic trees.
 *   -match_re:  Set to a regular that can be used for matching the node
 *               name with the feature name if the node name is not
 *               identical to the feature name.
 * @param $vocab
 *   Optional. An array containing a set of key/value pairs that maps node
 *   types to CV terms.  The keys must be 'root', 'internal' or 'leaf'.  If
 *   no vocab is provded then the terms provided by the tripal_phylogeny
 *   CV will be used.
 * @param $parent
 *   This argument is not needed when the funtion is first called. This
 *   function is recursive and this argument is used on recursive calls.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_phylogeny_import_tree(&$tree, $phylotree, $options, $vocab = [], $parent = NULL) {
  return chado_phylogeny_import_tree($tree, $phylotree, $options, $vocab, $parent);
}

/**
 * Get the vocabulary terms used to describe nodes in the tree.
 *
 * @return
 *  Array of vocab info or FALSE on failure.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_phylogeny_get_node_types_vocab() {
  return chado_phylogeny_get_node_types_vocab();
}

/**
 * Imports a tree file.
 *
 * This function is used as a wrapper for loading a phylogenetic tree using
 * any number of file loaders.
 *
 * @param $file_name
 *   The name of the file containing the phylogenetic tree to import.
 * @param $format
 *   The format of the file. Currently only the 'newick' file format is
 *   supported.
 * @param $options
 *   Options if the phylotree record already exists:
 *     'phylotree_id': The imported nodes will be associated with this tree.
 *     'leaf_type':  A sequence ontology term or the word 'organism'. If the
 *                   type is 'organism' then this tree represents a
 *                   taxonomic tree.  The default, if not specified, is the
 *                   term 'polypeptide'.
 *     'name_re':    If the leaf type is NOT 'taxonomy', then the value of
 *                   this field can be a regular expression to pull out
 *                   the name of the feature from the node label in the
 *                   intput tree. If no value is provided the entire label is
 *                   used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_phylogeny_import_tree_file($file_name, $format, $options = [], $job_id = NULL) {
  return chado_phylogeny_import_tree_file($file_name, $format, $options, $job_id);
}

/** tripal_pub_api */
/**
 * Retrieves a chado publication array.
 *
 * @param $identifier
 *   An array used to uniquely identify a publication. This array has the same
 *   format as that used by the chado_generate_var(). The following keys can be
 *   useful for uniquely identifying a publication as they should be unique:
 *    - pub_id: the chado pub.pub_id primary key.
 *    - nid: the drupal nid of the publication.
 *    - uniquename: A value to matach with the pub.uniquename field.
 *   There are also some specially handled keys. They are:
 *    - property: An array describing the property to select records for. It
 *      should at least have either a 'type_name' key (if unique across cvs) or
 *      'type_id' key. Other supported keys include: 'cv_id', 'cv_name'
 *      (of the type), 'value' and 'rank'
 *    - dbxref: The database cross reference accession.  It should be in the
 *      form DB:ACCESSION, where DB is the database name and ACCESSION is the
 *      unique publication identifier (e.g. PMID:4382934)
 *    - dbxref_id:  The dbxref.dbxref_id of the publication.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the pub record to
 * be returned.
 *
 * @return
 *   If a singe publication is retreived using the identifiers, then a
 *   publication array will be returned.  The array is of the same format
 *   returned by the chado_generate_var() function. Otherwise, FALSE will be
 *   returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_publication($identifiers, $options = []) {
  return chado_get_publication($identifiers, $options);
}

/**
 * The publication table of Chado only has a unique constraint for the
 * uniquename of the publication, but in reality a publication can be considered
 * unique by a combination of the title, publication type, published year and
 * series name (e.g. journal name or conference name). The site administrator
 * can configure how publications are determined to be unique.  This function
 * uses the configuration specified by the administrator to look for
 * publications that match the details specified by the $pub_details argument
 * and indicates if one ore more publications match the criteria.
 *
 * @param $pub_details
 *   An associative array with details about the publications. The expected
 *   keys
 *   are:
 *     'Title':              The title of the publication.
 *     'Year':               The published year of the publication.
 *     'Publication Type':   An array of publication types. A publication can
 *                           have more than one type.
 *     'Series Name':        The series name of the publication.
 *     'Journal Name':       An alternative to 'Series Name'.
 *     'Conference Name':    An alternative to 'Series Name'.
 *     'Citation':           The publication citation (this is the value saved
 *                           in the pub.uniquename field and must be unique).
 *
 *     If this key is present it will also be checked
 *     'Publication Dbxref': A database cross reference of the form
 *   DB:ACCESSION
 *                           where DB is the name of the database and ACCESSION
 *                           is the unique identifier (e.g PMID:3483139).
 *
 * @return
 *   An array containing the pub_id's of matching publications. Returns an
 *   empty array if no pubs match.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_publication_exists($pub_details) {
  return chado_publication_exists($pub_details);
}

/**
 * Used for autocomplete in forms for identifying for publications.
 *
 * @param $field
 *   The field in the publication to search on.
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_autocomplete_pub($string = '') {
  return chado_autocomplete_pub($string);
}

/**
 * Imports a singe publication specified by a remote database cross reference.
 *
 * @param $pub_dbxref
 *   The unique database ID for the record to update.  This value must
 *   be of the format DB_NAME:ACCESSION where DB_NAME is the name of the
 *   database (e.g. PMID or AGL) and the ACCESSION is the unique identifier
 *   for the record in the database.
 * @param $do_contact
 *   Set to TRUE if authors should automatically have a contact record added
 *   to Chado.
 * @param $do_update
 *   If set to TRUE then the publication will be updated if it already exists
 *   in the database.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_import_pub_by_dbxref($pub_dbxref, $do_contact = FALSE, $do_update = TRUE) {
  return chado_import_pub_by_dbxref($pub_dbxref, $do_contact, $do_update);
}

/**
 * Imports all publications for all active import setups.
 *
 * @param $report_email
 *   A list of email address, separated by commas, that should be notified
 *   once importing has completed.
 * @param $do_update
 *   If set to TRUE then publications that already exist in the Chado database
 *   will be updated, whereas if FALSE only new publications will be added.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_execute_active_pub_importers($report_email = FALSE, $do_update = FALSE) {
  return chado_execute_active_pub_importers($report_email, TRUE, $do_update);
}

/**
 * Updates publication records.
 *
 * Updates publication records that currently exist in the Chado pub table
 * with the most recent data in the remote database.
 *
 * @param $do_contact
 *   Set to TRUE if authors should automatically have a contact record added
 *   to Chado. Contacts are added using the name provided by the remote
 *   database.
 * @param $dbxref
 *   The unique database ID for the record to update.  This value must
 *   be of the format DB_NAME:ACCESSION where DB_NAME is the name of the
 *   database (e.g. PMID or AGL) and the ACCESSION is the unique identifier
 *   for the record in the database.
 * @param $db
 *   The name of the remote database to update.  If this value is provided and
 *   no dbxref then all of the publications currently in the Chado database
 *   for this remote database will be updated.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_reimport_publications($do_contact = FALSE, $dbxref = NULL, $db = NULL) {
  return chado_reimport_publications($do_contact, $dbxref, $db);
}

/**
 * Launch the Tripal job to generate citations.
 *
 * This function will recreate citations for all publications currently
 * loaded into Tripal.  This is useful to create a consistent format for
 * all citations.
 *
 * @param $options
 *  Options pertaining to what publications to generate citations for.
 *  One of the following must be present:
 *   - all: Create and replace citation for all pubs.
 *   - new: Create citation for pubs that don't already have one.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_pub_create_citations($options) {
  return chado_pub_create_citations($options);
}

/**
 * This function generates citations for publications.  It requires
 * an array structure with keys being the terms in the Tripal
 * publication ontology.  This function is intended to be used
 * for any function that needs to generate a citation.
 *
 * @param $pub
 *   An array structure containing publication details where the keys
 *   are the publication ontology term names and values are the
 *   corresponding details.  The pub array can contain the following
 *   keys with corresponding values:
 *     - Publication Type:  an array of publication types. a publication can
 *       have more than one type.
 *     - Authors: a  string containing all of the authors of a publication.
 *     - Journal Name:  a string containing the journal name.
 *     - Journal Abbreviation: a string containing the journal name
 *   abbreviation.
 *     - Series Name: a string containing the series (e.g. conference
 *       proceedings) name.
 *     - Series Abbreviation: a string containing the series name abbreviation
 *     - Volume: the serives volume number.
 *     - Issue: the series issue number.
 *     - Pages: the page numbers for the publication.
 *     - Publication Date:  A date in the format "Year Month Day".
 *
 * @return
 *   A text string containing the citation.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_pub_create_citation($pub) {
  return chado_pub_create_citation($pub);
}

/**
 * Retrieves the minimal information to uniquely describe any publication.
 *
 * The returned array is an associative array where the keys are
 * the controlled vocabulary terms in the form [vocab]:[accession].
 *
 * @param $pub
 *   A publication object as created by chado_generate_var().
 *
 * @return
 *   An array with the following keys:  'Citation', 'Abstract', 'Authors',
 *   'URL'. All keys are term names in the Tripal Publication Ontology :TPUB.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_minimal_pub_info($pub) {
  return chado_get_minimal_pub_info($pub);
}


/** tripal_stock_api */
/**
 * Retrieves a chado stock variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only
 *   one of the following unique keys is required):
 *    - stock_id: the chado stock.stock_id primary key
 *    - nid: the drupal nid of the stock
 *   There are also some specially handled keys. They are:
 *    - property: An array/object describing the property to select records for.
 *      It should at least have either a type_name (if unique across cvs) or
 *      type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *      definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the stock record
 * to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the stock will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_stock($identifiers, $options = []) {
  return chado_get_stock($identifiers, $options);
}

/**
 * Retrieves a chado stock variable.
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys
 *   include any field in the stock table. See the chado_select_record() $values
 *   parameter for additional details including an example.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * @return
 *   An array of stock objects matching the criteria.
 *
 * @ingroup tripal_chado_module_DEPRECATED_api
 */
function tripal_get_multiple_stocks($identifiers, $options = []) {
  return chado_get_multiple_stocks($identifiers, $options);
}
