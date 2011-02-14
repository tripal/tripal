<?php

/*************************************************************************
*
*/
function tripal_core_bulk_loader_create (){
  
   return drupal_get_form('tripal_core_bulk_loader_create_form');
}
/*************************************************************************
*
*/
function tripal_core_bulk_loader_create_form (&$form_state = NULL){
   $form = array();  

   // get the step used by this multistep form
   $step = 1;
   if(isset($form_state['storage'])){
      $step = (int)$form_state['storage']['step'];
   }  

   $form = array();
   if($step == 1){
      tripal_core_bulk_loader_create_form_step1 ($form,$form_state);
      $form_state['storage']['step'] = $step + 1;
   }
   if($step == 2){
      tripal_core_bulk_loader_create_form_step2 ($form,$form_state);
   }

   return $form;
}
/*************************************************************************
*
*/
function tripal_core_bulk_loader_create_form_validate ($form,&$form_state) {


}
/*************************************************************************
*
*/
function tripal_core_bulk_loader_create_form_submit ($form,&$form_state) {
   if($form_State['storage']['step'] < 4){
      return;
   }
}

/*************************************************************************
*
*/
function tripal_core_bulk_loader_create_form_step2 (&$form,$form_state){

   if(isset($form_state['values']['columns'])){
      $form_state['storage']['columns'] = $form_state['values']['columns'];
   }

   $form['bulk_loader'] = array(
      '#type' => 'fieldset',
      '#title' => t('Step 2: Define the columns of the file'),
   );

   $columns = $form_state['storage']['columns'];
//   $form['debug']= array(
//      '#value' => "Columns: $columns",
//      '#weight'        => -1
//   );

	$fields = array();
   $fields[''] = '';

   // these fields correspond with the columns of the feature table and
   // the foreign key contraints for the feature table.
   $fields = array(
        '' => '',
        'ignore' => 'Ignore this column',
        'Feature details' => array (
           'name' => 'Feature Name (human readable)',
           'uniquename' => 'Unique Feature Name',
           'type' => 'Feature type (must be a valid SO term)',
           'alt_type' => 'Additional ontology term',
           'residues' => 'Residues',
         ),
        'Organism specification' => array (
           'organism_id' => 'Organism ID number',
           'genus' => 'Genus',
           'species' => 'Species',
           'full_name' => 'Scientific Name (genus + species)',
           'common_name' => 'Common Name',
         ),
        'External Database Cross-Reference' => array(
           'db_id' => 'Database ID number',
           'db_name' => 'Database name (must exists in the database)',
           'accession' => 'Accession',
         ),
        'Feature Relationship' => array (
            'rel_type' => 'Relationship Type (must be a valid relationship term)',
            'parent' => 'Parent unique name',
            'parent type' => 'Parent Type (must be a valid SO term)',
         ),
        'Feature Location' => array (
           'srcfeature' => 'Reference feature (must already exist in the database)',
           'srcfeature_type' => 'Reference feature type (must be a valid SO term)',
           'fmin' => 'Start position',
           'fmax' => 'Stop position',
           'strand' => 'Strand (valid values: 0,-1,+1)',
           'phase' => 'Phase (valid values: (+,-,.)',
         ),
         'Feature Property' => array (
            'property' => 'Feature property value',
         ),
         'Feature Synonym' => array (
            'syn_name' => 'Synonym name',
         ),
         'Library specification' => array (
            'library_id' => 'Library ID number',
            'library_name' => 'Library name',
         ),
         'Analysis specification' => array (
            'analysis_id' => 'Analysis ID number',
            'analysis_source' => 'Analysis identifying name (sourcename column in Chado)',
            'analysis_desc' => 'Analysis description',
            'analysis_program' => 'Analysis program',
            'analysis_program_version' => 'Analysis program version'
         ),
   );

   // organism foreign key identifies.  These are used to find the organism
   // for which the feature belongs.
   $form['columns'] = array(
      '#type' => 'hidden',
      '#value' => $columns,
   );


   // get the list of organisms
   $sql = "SELECT * FROM {organism} ORDER BY genus, species";
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $org_rset = db_query($sql);
   tripal_db_set_active($previous_db);  // now use drupal database
   $organisms = array();
   $genus = array();
   $species = array();
   $common_names = array();
   $genera[''] = '';
   $species[''] = '';
   $common_names[''] = '';
   $full_names[''] = '';
   while($organism = db_fetch_object($org_rset)){
      $full_names["$organism->genus $organism->species"] = "$organism->genus $organism->species";
      $genera[$organism->genus] = "$organism->genus";
      $species[$organism->species] = "$organism->species";
      $common_names[$organism->common_name] = "$organism->common_name";
   }

   for($i = 1; $i <= $columns; $i++){
      $form['bulk_loader']["col_$i"] = array(
         '#type' => 'fieldset',
         '#title' => t("Column $i of the input file"),
      );
      $form['bulk_loader']["col_$i"]["col_type_$i"] = array(
       '#title'         => t('Field Selection'),
       '#type'          => 'select',
       '#options'       => $fields,
       '#weight'        => 0,
        // this field will use AJAX to populate and information needed
        // for specific types, such as for feature properties.  It
        // calls the step2_get_type URL and passes the column ($i).  
        // the return value is additional form items or text
       '#ahah'          => array(
          'event' => 'change',
          'wrapper' => "type_cols_$i",
          'path' => "/admin/tripal/bulk_load/step2_get_type/$i",
          'effect' => 'fade',
          'method' => 'replace'
        ),
 	   );

     // these next fields are hidden (access = false) and will be shown
     // if the user selects the feature property type in the drop down
     // above.  


     //-------------------------------------------------------------------------
     // default text box for allowed values
     $form['bulk_loader']["col_$i"]["col_prop_valid_$i"] = array(
       '#title'         => t('Allowed Values'),
       '#type'          => 'textarea',
       '#weight'        => 0,
       '#description'   => 'Please provide a list of allowed values for this field. Separate these values with a space. Leave blank to allow any value',
       '#access'        => FALSE
     );

     //-------------------------------------------------------------------------
     // Organism allowed values
     $form['bulk_loader']["col_$i"]["col_prop_genera_$i"] = array (
        '#title'       => t('Allowed Values'),
        '#type'        => t('select'),
        '#description' => t("Choose all allowed genera values for this column (ctrl+click to select multiple values). Select none to allow all"),
        '#required'    => FALSE,
        '#options'     => $genera,
        '#weight'      => 2,
        '#multiple'    => TRUE,
        '#size'        => 10,
        '#access'        => FALSE
      );
     $form['bulk_loader']["col_$i"]["col_prop_species_$i"] = array (
        '#title'       => t('Allowed Values'),
        '#type'        => t('select'),
        '#description' => t("Choose all allowed species values for this column (ctrl+click to select multiple values). Select none to allow all"),
        '#required'    => FALSE,
        '#options'     => $species,
        '#weight'      => 2,
        '#multiple'    => TRUE,
        '#size'        => 10,
        '#access'        => FALSE
      );
     $form['bulk_loader']["col_$i"]["col_prop_common_name_$i"] = array (
        '#title'       => t('Allowed Values'),
        '#type'        => t('select'),
        '#description' => t("Choose all allowed values for this column (ctrl+click to select multiple values). Select none to allow all"),
        '#required'    => FALSE,
        '#options'     => $common_names,
        '#weight'      => 2,
        '#multiple'    => TRUE,
        '#size'        => 10,
        '#access'        => FALSE
      );
     $form['bulk_loader']["col_$i"]["col_prop_full_name_$i"] = array (
        '#title'       => t('Allowed Values'),
        '#type'        => t('select'),
        '#description' => t("Choose all allowed values for this column (shift+click to select multiple values). Select none to allow all"),
        '#required'    => FALSE,
        '#options'     => $full_names,
        '#weight'      => 2,
        '#multiple'    => TRUE,
        '#size'        => 10,
        '#access'        => FALSE
      );


     //-------------------------------------------------------------------------
     // feature property fields
     $form['bulk_loader']["col_$i"]["col_prop_name_$i"] = array(
       '#title'         => t('Property Name'),
       '#type'          => 'textfield',
       '#weight'        => 1,
       '#description'   => 'Please provide a name for this property.  It should be a single 
                            word with only alphanumeric characters and underscores.  If this 
                            name exists as a CV term in Chado already it will be reused, otherwise
                            a new CV term with this name will be added.',
       '#required'      => TRUE,
       '#access'        => FALSE
     );
     $form['bulk_loader']["col_$i"]["col_prop_full_name_$i"] = array(
       '#title'         => t('Property Full Name'),
       '#type'          => 'textfield',
       '#weight'        => 3,
       '#description'   => 'Please provide a human readable name for this property.  This will be used when
                            displaying the property title',
       '#access'        => FALSE
     );
     $form['bulk_loader']["col_$i"]["col_prop_desc_$i"] = array(
       '#title'         => t('Property Description'),
       '#type'          => 'textarea',
       '#weight'        => 4,
       '#description'   => 'Please provide a description of this property.',
       '#access'        => FALSE
     );

     // this is an empty div box that gets put on the form.  This is the
     // box where the hidden form elements will be rendered when the
     // AJAX call returns
     $form['bulk_loader']["col_$i"]["type_wrap_$i"] = array(        
            '#prefix' => "<div class=\"clear-block\" id=\"type_cols_$i\">",
            '#value' => ' ',             
            '#suffix' => '</div>',
        );
   }

   
   $form['submit'] = array (
     '#type'         => 'submit',
     '#value'        => t('Next'),
     '#weight'       => 5,
     '#executes_submit_callback' => TRUE,
   );   
}
/*************************************************************************
*
*/

function tripal_core_bulk_loader_ahah_step2_feature_type($column){


  // create a form_state array to hold the form variables
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_state['storage']['step'] = 2;
  
  // retreive the form from the cache
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);

  // get the form name
  $args = $form['#parameters'];
  $form_id = array_shift($args); 

  // add the post variables to the form and set a few other items
  $form['#post'] = $_POST;
  $form['#redirect'] = FALSE;
  $form['#programmed'] = FALSE;
  $form_state['post'] = $_POST;

  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  // get the column
  $i = $column;

  // check to see what type of field has been selected and provide the
  // additional fields as appropriate
  $type = $form_state["post"]["col_type_$i"];
  if(strcmp($type,'genus')==0){
     $propform['bulk_loader']["col_$i"]["col_prop_genera_$i"] = $form['bulk_loader']["col_$i"]["col_prop_genera_$i"];
     $propform['bulk_loader']["col_$i"]["col_prop_genera_$i"]['#access'] = TRUE;

  }
  elseif(strcmp($type,'species')==0){
     $propform['bulk_loader']["col_$i"]["col_prop_species_$i"] = $form['bulk_loader']["col_$i"]["col_prop_species_$i"];
     $propform['bulk_loader']["col_$i"]["col_prop_species_$i"]['#access'] = TRUE;

  }
  elseif(strcmp($type,'common_name')==0){
     $propform['bulk_loader']["col_$i"]["col_prop_common_name_$i"] = $form['bulk_loader']["col_$i"]["col_prop_common_name_$i"];
     $propform['bulk_loader']["col_$i"]["col_prop_common_name_$i"]['#access'] = TRUE;

  }
  elseif(strcmp($type,'full_name')==0){
     $propform['bulk_loader']["col_$i"]["col_prop_full_name_$i"] = $form['bulk_loader']["col_$i"]["col_prop_full_name_$i"];
     $propform['bulk_loader']["col_$i"]["col_prop_full_name_$i"]['#access'] = TRUE;

  }
  elseif(strcmp($type,'property')==0){
     // we just want to render the property fields that were previously not visible
     $propform['bulk_loader']["col_$i"]["col_prop_name_$i"] = $form['bulk_loader']["col_$i"]["col_prop_name_$i"];
     $propform['bulk_loader']["col_$i"]["col_prop_full_name_$i"] = $form['bulk_loader']["col_$i"]["col_prop_full_name_$i"];
     $propform['bulk_loader']["col_$i"]["col_prop_desc_$i"] = $form['bulk_loader']["col_$i"]["col_prop_desc_$i"];
     $propform['bulk_loader']["col_$i"]["col_prop_name_$i"]['#access'] = TRUE;
     $propform['bulk_loader']["col_$i"]["col_prop_full_name_$i"]['#access'] = TRUE;
     $propform['bulk_loader']["col_$i"]["col_prop_desc_$i"]['#access'] = TRUE;
  } 
  else {
     // use a default valid values textbox if we have no special needs for the type
     $propform['bulk_loader']["col_$i"]["col_prop_valid_$i"] = $form['bulk_loader']["col_$i"]["col_prop_valid_$i"];
     $propform['bulk_loader']["col_$i"]["col_prop_valid_$i"]['#access'] = TRUE;
  }

  $output = theme('status_messages') . drupal_render($propform);

  // Final rendering callback.
  drupal_json(array('status' => TRUE, 'data' => $output));
}
/*************************************************************************
*
*/
function tripal_core_bulk_loader_create_form_step1 (&$form,$form_state){
	$modules = array();
   $modules['feature'] = 'Feature';
   $modules['organism'] = 'Organism';
   $modules['library'] = 'Library';
   $modules['analysis'] = 'Analysis';


   // set the fieldset title to indicate the step
   $form['bulk_loader'] = array(
      '#type' => 'fieldset',
      '#title' => t('Step 1: Select the Chado module'),
   );

	$form['bulk_loader']['chado_module'] = array(
      '#title'         => t('Chado Module'),
      '#description'   => t('Please select the module for which you would like to create an importer'),
      '#type'          => 'select',
      '#options'       => $modules,
      '#weight'        => 0,
      '#required'      => TRUE
	);

   $form['bulk_loader']['columns']= array(
      '#type'          => 'textfield',
      '#title'         => t('Number of Columns'),
      '#description'   => t('Please specify the number of columns in the input file.'),
      '#weight'        => 2,
      '#required'      => TRUE
   );

   $form['submit'] = array (
     '#type'         => 'submit',
     '#value'        => t('Next'),
     '#weight'       => 5,
     '#executes_submit_callback' => TRUE,
   );   
}
