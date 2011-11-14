<?php

function tripal_analysis_go_gaf_load_form (){

   $form['notice']= array(
      '#type' => 'item',
      '#value' => t('Note: currently, the GAF loader only uses column 2 (Object ID) and 5 (GO ID) 
          from the GAF file, and simply imports GO terms for the features. 
          Further support for this file format will be provided later.'),
   );

   $form['gaf_file']= array(
      '#type'          => 'textfield',
      '#title'         => t('GAF File'),
      '#description'   => t('Please enter the full system path for the GAF file, or a path within the Drupal
                             installation (e.g. /sites/default/files/xyz.txt).  The path must be accessible to the
                             server on which this Drupal instance is running.'),
      '#required' => TRUE,
   );
   // get the list of organisms
   $sql = "SELECT * FROM {organism} ORDER BY genus, species";
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $org_rset = db_query($sql);
   tripal_db_set_active($previous_db);  // now use drupal database
   $organisms = array();
   $organisms[''] = '';
   while($organism = db_fetch_object($org_rset)){
      $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
   }
   $form['organism_id'] = array (
     '#title'       => t('Organism'),
     '#type'        => t('select'),
     '#description' => t("Choose the organism to which these sequences are associated "),
     '#required'    => TRUE,
     '#options'     => $organisms,
   );
   $form['seq_type']= array(
      '#type' => 'textfield',
      '#title' => t('Sequence Type'),
      '#required' => TRUE,
      '#description' => t('Please enter the Sequence Ontology term that describes the sequences in the GAF file.'),
   );
	$form['query_uniquename'] = array(
      '#title' => t('Use Unique Name'),
      '#type' => 'checkbox',
      '#description' => t('Select this checboxk if the feature name in the GAF file '.
        'matches the uniquename in the database.  By default, the feature will '.
        'be mapped to the "name" of the feature.'),
      '#default_value' => $query_uniquename,
	);
   $form['import_options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Import Options'),
      '#collapsed' => TRUE
   );
   $form['import_options']['add_only']= array(
      '#type' => 'checkbox',
      '#title' => t('Add GO terms'),
      '#required' => FALSE,
      '#description' => t('GO terms in the GAF file will be added to each feature.'),
   );
//   $form['import_options']['replace']= array(
//      '#type' => 'checkbox',
//      '#title' => t('Replace GO terms'),
//      '#required' => FALSE,
//      '#description' => t('All GO terms for features in the GAF file will be replaced with terms in the GAF file.'),
//   );
   $form['import_options']['remove']= array(
      '#type' => 'checkbox',
      '#title' => t('Delete GO terms'),
      '#required' => FALSE,
      '#description' => t('GO terms for features in the GAF file will be removed. Other terms will remain.'),
   );

   $form['analysis'] = array(
      '#type' => 'fieldset',
      '#title' => t('Analysis Used to Derive GO terms'),
      '#collapsed' => TRUE
   ); 

   // get the list of organisms
   $sql = "SELECT * FROM {analysis} ORDER BY name";
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $org_rset = db_query($sql);
   tripal_db_set_active($previous_db);  // now use drupal database
   $analyses = array();
   $analyses[''] = '';
   while($analysis = db_fetch_object($org_rset)){
      $analyses[$analysis->analysis_id] = "$analysis->name ($analysis->program $analysis->programversion, $analysis->sourcename)";
   }
   $form['analysis']['analysis_id'] = array (
     '#title'       => t('Analysis'),
     '#type'        => t('select'),
     '#description' => t("Choose the analysis that defines how the GO annotations in the GAF file were created. "),
     '#required'    => TRUE,
     '#options'     => $analyses,
   );


  // Advanced Options
   $form['advanced'] = array(
      '#type' => 'fieldset',
      '#title' => t('Advanced Options'),
      '#collapsed' => TRUE
   );
   $form['advanced']['re_help']= array(
      '#type' => 'item',
      '#value' => t('A regular expression is an advanced method for extracting information from a string of text.  
                     By default, this loader will use the first word in the second column of the GAF file 
                     as the uniquename for the sequences.  If this is not desired, you may use the following regular 
                     expressions to define the location of the name or unique name within the text of column 2.'),
   );

   $form['advanced']['re_name']= array(
      '#type' => 'textfield',
      '#title' => t('Regular expression for the name'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the '.
         'feature name from the GAF file. This option is '.
         'is only required when the feature identifier does not identically match a feature '.
         'in the database.'),
   );   
 

   $form['button'] = array(
      '#type' => 'submit',
      '#value' => t('Import GAF file'),
   );
   return $form;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_analysis_go_gaf_load_form_validate ($form, &$form_state){

   $gaf_file = $form_state['values']['gaf_file'];
   $organism_id = $form_state['values']['organism_id'];
   $add_only = $form_state['values']['add_only'];
   $remove   = $form_state['values']['remove'];
   $replace  = $form_state['values']['replace'];
   $analysis_id = $form_state['values']['analysis_id'];
   $type      = trim($form_state['values']['seq_type']);
   $re_name      = trim($form_state['values']['re_name']);
   $query_uniquename = $form_state['values']['query_uniquename'];


   // check to see if the file is located local to Drupal
   $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $gaf_file; 
   if(!file_exists($dfile)){
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      $dfile = $gaf_file;
   }
   if(!file_exists($dfile)){
      form_set_error('gff_file',t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
   }

   if (($add_only and ($remove or $replace)) or 
       ($replace and ($add_only or $remove)) or
       ($remove and ($replace or $add_only))){
       form_set_error('add_only',t("Please select only one checkbox from the import options section"));
   }

   // check to make sure the types exists
   $cvtermsql = "SELECT CVT.cvterm_id
                 FROM {cvterm} CVT
                    INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
                    LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
                 WHERE cv.name = '%s' and (CVT.name = '%s' or CVTS.synonym = '%s')";
   $cvterm = db_fetch_object(db_query($cvtermsql,'sequence',$type,$type));
   if(!$cvterm){
      form_set_error('type',t("The Sequence Ontology (SO) term selected for the sequence type is not available in the database. Please check spelling or select another."));
   }
   if($rel_type){
      $cvterm = db_fetch_object(db_query($cvtermsql,'sequence',$parent_type,$parent_type));
      if(!$cvterm){
         form_set_error('parent_type',t("The Sequence Ontology (SO) term selected for the parent relationship is not available in the database. Please check spelling or select another."));
      }
   }
}

/**
 *
 * @ingroup gff3_loader
 */
function tripal_analysis_go_gaf_load_form_submit ($form, &$form_state){
   global $user;

   $gaf_file = $form_state['values']['gaf_file'];
   $organism_id = $form_state['values']['organism_id'];
   $add_only = $form_state['values']['add_only'];
   $remove   = $form_state['values']['remove'];
   $replace  = $form_state['values']['replace'];
   $analysis_id = $form_state['values']['analysis_id'];
   $type      = trim($form_state['values']['seq_type']);
   $re_name      = trim($form_state['values']['re_name']);
   $query_uniquename = $form_state['values']['query_uniquename'];

   $args = array($gaf_file,$organism_id,$analysis_id,$add_only,$replace,
      $remove,$re_name,$type,$query_uniquename);
   if($add_only){
     $type = 'add GO terms';
   }
   if($replace){
     $type = 'replace and add GO terms';
   }
   if($remove){
     $type = 'remove GO terms';
   }
   tripal_add_job("Import GAF 2.0 file $gaf_file and $type",'tripal_analysis_go',
      'tripal_analysis_go_load_gaf',$args,$user->uid);

   return '';
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_analysis_go_load_gaf($gaf_file, $organism_id,$analysis_id,$add_only =0, 
   $replace = 0, $remove = 0, $re_name, $type, $query_uniquename,
   $job = NULL)
{
   print "Opening GAF file $gaf_file\n";

    
   $lines = file($gaf_file,FILE_SKIP_EMPTY_LINES);
   $i = 0;

   $name = '';
   $residues = '';
   $num_lines = sizeof($lines);
   $interval = intval($num_lines * 0.01);
   if($interval == 0){
      $interval = 1;
   }

   foreach ($lines as $line_num => $line) {
      $i++;  // update the line count     

      // skip comments
      if(preg_match('/^\!/',$line)){
         continue; 
      }

      // update the job status every 1% features
      if($job and $i % $interval == 0){
         tripal_job_set_progress($job,intval(($i/$num_lines)*100));
      }

      // split the line into it's columns
      $cols = explode("\t",$line);
      if(sizeof($cols) < 15){
         print "ERROR: improper number of columns on line $i\n";
         print_r($cols);
         return '';
      }
      $db        = $cols[0];
      $object    = $cols[1];
      $symbol    = $cols[2];
      $qualifier = $cols[3];
      $go_id     = $cols[4];
      $dbxref    = $cols[5];
      $ecode     = $cols[6];
      $with      = $cols[7];
      $aspect    = $cols[8];
      $obj_name  = $cols[9];
      $obj_syn   = $cols[10];
      $obj_type  = $cols[11];
      $taxon     = $cols[12];
      $date      = $cols[13];
      $assigned  = $cols[14];
      $exten     = $cols[15];
      $product   = $cols[16];

      // get the name or uniquename for the feature
      $name = $object;
      if($re_name){
         if(!preg_match("/$re_name/",$object,$matches)){
            print "Regular expression for the feature name finds nothing\n";
         } else {
            $name = trim($matches[1]);
         }
      } else {
         if(preg_match("/^\s*(.*?)[\s\|].*$/",$object,$matches)){
            $name = trim($matches[1]);
         }
      }
      

      // get the feature
      $values = array(
         'type_id' => array(
            'cv_id' => array(
               'name' => 'sequence'
            ),
            'name' => $type,
         ),
         'organism_id' => $organism_id,
      );
      if(!$query_uniquename){
        $values['name'] = $name;
      } else {
        $values['uniquename'] = $name;
      } 
      $feature = tripal_core_chado_select('feature',array('*'),$values);
      if(count($feature) == 0){
         print "WARNING: Cannot find the feature: '$name'\n";         
      } else {
         // add the GO term to the feature
         tripal_analysis_go_load_gaff_go_term($feature[0],$go_id,$remove,$analysis_id);
      }
   }
   return 1;
}
/**
*
*/
function tripal_analysis_go_load_gaff_go_term($feature,$dbxref,$remove,$analysis_id){

   // get the database name from the reference.  If it doesn't exist then create one.
   $ref = explode(":",$dbxref);
   $dbname = $ref[0];
   $accession = $ref[1];

   // first look for the database name 
   $db = tripal_core_chado_select('db',array('db_id'),array('name' => "DB:$dbname"));  
   if(sizeof($db) == 0){
      $db = tripal_core_chado_select('db',array('db_id'),array('name' => "$dbname"));      
   }        
   if(sizeof($db) == 0){
      print "ERROR: Database, $dbname is missing for reference: $dbname:$accession\n";
      return 0;
   } 
   $db = $db[0];
    
   // now check to see if the accession exists
   $dbxref = tripal_core_chado_select('dbxref',array('dbxref_id'),array(
      'accession' => $accession,'db_id' => $db->db_id));
   if(sizeof($dbxref) == 0){
      print "ERROR: Accession, $accession is missing for reference: $dbname:$accession\n";
      return 0;
   }
   $dbxref = $dbxref[0];

   // now check to see if the cvterm exists
   $cvterm = tripal_core_chado_select('cvterm',array('cvterm_id'),array(
      'dbxref_id' => $dbxref->dbxref_id));
   // if it doesn't exist in the cvterm table, look for an alternate id
   if(sizeof($cvterm) == 0){
      $cvterm = tripal_core_chado_select('cvterm_dbxref',array('cvterm_id'),array(
         'dbxref_id' => $dbxref->dbxref_id));
   }
   if(sizeof($cvterm) == 0){
      print "ERROR: CVTerm is missing for reference: $dbname:$accession\n";
      return 0;
   }
   $cvterm = $cvterm[0];
   

   // check to see if this feature cvterm already exists
   $fcvt = tripal_core_chado_select('feature_cvterm',array('feature_cvterm_id'),
      array('cvterm_id' => $cvterm->cvterm_id,'feature_id' => $feature->feature_id));

   // now associate this feature with the cvterm if it doesn't already exist
   if(sizeof($fcvt)==0){
      $values = array(
         'feature_id' => $feature->feature_id,
         'cvterm_id' => $cvterm->cvterm_id,
         'pub_id' => array(
            'uniquename' => 'null',
         ),
      );
      $ret = tripal_core_chado_insert('feature_cvterm',$values);

      if($ret){
         print "   Added ontology term $dbname:$accession to feature $feature->uniquename\n";
      } else {
         print "ERROR: failed to insert ontology term '$dbname:$accession' for feature: $feature\n";
         return 0;
      }
   } else {
      if($remove){
         $status = tripal_core_chado_delete('feature_cvterm',
            array('cvterm_id' => $cvterm->cvterm_id,
                 'feature_id' => $feature->feature_id));
         if(!$status){
            print "ERROR: Failed to delete ontology term $dbname:$accession from feature $feature->uniquename\n";
         } else {
            print "   Deleted ontology term $dbname:$accession from feature $feature->uniquename\n";
         }
      } else {
         print "   Ontology term already associated to feature $feature->uniquename, skipping $dbname:$accession\n";
      }
   }

   if(!$remove){
      print "   Associating feature $feature->name to analysis\n";
      // Insert into analysisfeature table only if it doesn't already exist
      $values = array('feature_id' => $feature->feature_id, 'analysis_id' => $analysis_id);
      $analysisfeature = tripal_core_chado_select('analysisfeature',array('*'),$values);
      if(sizeof($analysisfeature) == 0){
         $analysisfeature = tripal_core_chado_insert('analysisfeature',$values);
         $analysisfeature_id = $analysisfeature['analysisfeature_id'];
      } else {
         $analysisfeature_id = $analysisfeature[0]->analysisfeature_id;
      }

      // Insert GO terms into analysisfeatureprop table
      $values = array('analysisfeature_id' => $analysisfeature_id,
                      'type_id' => $cvterm->cvterm_id,
                      'rank' => 0);
      $analysisfeatureprop = tripal_core_chado_select('analysisfeatureprop',array('*'),$values);
      if(sizeof($analysisfeatureprop) == 0){
         $values['value'] = $matches[1];
         $analysisfeatureprop = tripal_core_chado_insert('analysisfeatureprop',$values);
      }
   }
   return 1;
}
/**
*
*/
function tripal_analysis_go_load_gaff_insert_analysisfeatureprop ($feature_id, $analysis_id,
   $brite_id,$keggterm)
{

   // add the analysisfeature record if it doesn't already exist.
   $values = array('feature_id' => $feature_id,'analysis_id' => $analysis_id);
   $analysisfeature_arr = tripal_core_chado_select('analysisfeature',
      array('analysisfeature_id'),$values);
   if(count($analysisfeature_arr) == 0){
      tripal_core_chado_insert('analysisfeature',$values);
      $analysisfeature_arr = tripal_core_chado_select('analysisfeature',
         array('analysisfeature_id'),$values);
   }
   $analysisfeature_id = $analysisfeature_arr[0]->analysisfeature_id;            
   // Insert into analysisfeatureprop if the value doesn't already exist
   // KEGG heir results sometimes have the same record more than once.
   if($analysisfeature_id){
      // Get the highest rank for this feature_id in analysisfeatureprop table
      $sql = "SELECT rank FROM analysisfeatureprop WHERE analysisfeature_id = %d and type_id = %d ORDER BY rank DESC";
      $previous_db = tripal_db_set_active('chado');
      $result = db_fetch_object(db_query($sql,$analysisfeature_id,$brite_id));
      tripal_db_set_active($previous);
      $rank = 0;
      if ($result and $result->rank > 0) {
         $rank = $result->rank + 1;
      }

      $values = array(
         'analysisfeature_id' => $analysisfeature_id, 
         'type_id' => $brite_id,
         'value' => $keggterm,
         'rank' => $rank,
      );

      return tripal_core_chado_insert('analysisfeatureprop',$values);
   }
   else {
      return 0;
   }
}
