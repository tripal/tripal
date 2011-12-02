<?php

/**
 * @defgroup fasta_loader FASTA Feature Loader
 * @{
 * Provides fasta loading functionality. Creates features based on their specification in a fasta file.
 * @}
 * @ingroup tripal_feature
 */
 
/**
 *
 *
 * @ingroup fasta_loader
 */
function tripal_feature_fasta_load_form (){

   $form['fasta_file']= array(
      '#type'          => 'textfield',
      '#title'         => t('FASTA File'),
      '#description'   => t('Please enter the full system path for the FASTA file, or a path within the Drupal
                             installation (e.g. /sites/default/files/xyz.obo).  The path must be accessible to the
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

   $form['seqtype']= array(
      '#type' => 'textfield',
      '#title' => t('Sequence Type'),
      '#required' => TRUE,
      '#description' => t('Please enter the Sequence Ontology term that describes the sequences in the FASTA file.'),
   );


   // get the list of organisms
   $sql = "SELECT L.library_id, L.name, CVT.name as type
           FROM {library} L
              INNER JOIN {cvterm} CVT ON L.type_id = CVT.cvterm_id
           ORDER BY name";
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $lib_rset = db_query($sql);
   tripal_db_set_active($previous_db);  // now use drupal database
   $libraries = array();
   $libraries[''] = '';
   while($library = db_fetch_object($lib_rset)){
      $libraries[$library->library_id] = "$library->name ($library->type)";
   }
//   $form['library_id'] = array (
//     '#title'       => t('Library'),
//     '#type'        => t('select'),
//     '#description' => t("Choose the library to which these sequences are associated "),
//     '#required'    => FALSE,
//     '#options'     => $libraries,
//     '#weight'      => 5,
//   );
   $form['method']= array(
      '#type' => 'radios',
      '#title' => 'Method',
      '#required' => TRUE,
      '#options' => array(
         t('Insert only'),
         t('Update only'),
         t('Insert and update'),
      ),
      '#description' => t('Select how features in the FASTA file are handled.  
         Select "Insert only" to insert the new features. If a feature already 
         exists with the same name or unique name and type then it is skipped.
         Select "Update only" to only update featues that already exist in the
         database.  Select "Insert and Update" to insert features that do
         not exist and upate those that do.'),
      '#default_value' => 2,
   );

$form['match_type']= array(
      '#type' => 'radios',
      '#title' => 'Name Match Type',
      '#required' => TRUE,
      '#options' => array(
         t('Name'),
         t('Unique name'),
      ),
      '#description' => t('Feature data is stored in Chado with both a human-readable
        name and a unique name. If the features in your FASTA file are identified using
        a human-readable name then select the "Name" button. If your features are
        identified using the unique name then select the "Unique name" button.  If you 
        loaded your features first using the GFF loader then the unique name of each
        features were indicated by the "ID=" attribute and the name by the "Name=" attribute.
        By default, the FASTA loader will use the first word (character string
        before the first space) as  the name for your feature. If 
        this does not uniquely identify your feature consider specifying a regular expression in the advanced section below. 
        Additionally, you may import both a name and a unique name for each sequence using the advanced options. 
        When updating a sequence, the value selected here will be used to identify the sequence in the 
        database in combination with any regular expression provided below.'),
      '#default_value' => 1,
   );

   $form['analysis'] = array(
      '#type' => 'fieldset',
      '#title' => t('Analysis Used to Derive Features'),
      '#collapsed' => TRUE
   ); 
   $form['analysis']['desc'] = array(
      '#type' => 'markup',
      '#value' => t("Why specify an analysis for a data load?  All data comes 
         from some place, even if downloaded from Genbank. By specifying
         analysis details for all data uploads, it allows an end user to reproduce the
         data set, but at least indicates the source of the data."), 
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
     '#description' => t("Choose the analysis to which these features are associated "),
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
                     Your FASTA file may contain both a human-readable name and a unique name for each sequence.  
                     If you want to import
                     both the name and unique name for all sequences, then you must provide regular expressions 
                     so that the loader knows how to separate them.  
                     Otherwise the name and uniquename will be the same.  
                     By default, this loader will use the first word in the definition 
                     lines of the FASTA file
                     as the name or unique name of the feature.'),
   );
   $form['advanced']['re_name']= array(
      '#type' => 'textfield',
      '#title' => t('Regular expression for the name'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the 
         feature name from the FASTA definition line. For example, for a 
         defintion line with a name and unique name separated by a bar \'|\' (>seqname|uniquename), 
         the regular expression for the name would be, "^(.*?)\|.*$".'),
   );  
   $form['advanced']['re_uname']= array(
      '#type' => 'textfield',
      '#title' => t('Regular expression for the unique name'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the 
         feature name from the FASTA definition line. For example, for a 
         defintion line with a name and unique name separated by a bar \'|\' (>seqname|uniquename), 
         the regular expression for the unique name would be "^.*?\|(.*)$").'),
   );   
 

   // Advanced database cross-reference optoins
   $form['advanced']['db'] = array(
      '#type' => 'fieldset',
      '#title' => t('External Database Reference'),
      '#weight'=> 6,
      '#collapsed' => TRUE
   );
   $form['advanced']['db']['re_accession']= array(
      '#type' => 'textfield',
      '#title' => t('Regular expression for the accession'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the accession for the external database for each feature from the FASTA definition line.'),
      '#weight' => 2
   ); 

  // get the list of databases
   $sql = "SELECT * FROM {db} ORDER BY name";
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $db_rset = db_query($sql);
   tripal_db_set_active($previous_db);  // now use drupal database
   $dbs = array();
   $dbs[''] = '';
   while($db = db_fetch_object($db_rset)){
      $dbs[$db->db_id] = "$db->name";
   }
   $form['advanced']['db']['db_id'] = array (
     '#title'       => t('External Database'),
     '#type'        => t('select'),
     '#description' => t("Plese choose an external database for which these sequences have a cross reference."),
     '#required'    => FALSE,
     '#options'     => $dbs,
     '#weight'      => 1,
   );

   $form['advanced']['relationship'] = array(
      '#type' => 'fieldset',
      '#title' => t('Relationships'),
      '#weight'=> 6,
      '#collapsed' => TRUE
   );
   $rels = array();
   $rels[''] = '';
   $rels['part_of'] = 'part of';
   $rels['derives_from'] = 'produced by';


   // Advanced references options
   $form['advanced']['relationship']['rel_type']= array(
     '#title'       => t('Relationship Type'),
     '#type'        => t('select'),
     '#description' => t("Use this option to create associations, or relationships between the 
                          features of this FASTA file and existing features in the database. For 
                          example, to associate a FASTA file of peptides to existing genes or transcript sequence, 
                          select the type 'produced by'. For a CDS sequences select the type 'part of'"),
     '#required'    => FALSE,
     '#options'     => $rels,
     '#weight'      => 5,
   );
   $form['advanced']['relationship']['re_subject']= array(
      '#type' => 'textfield',
      '#title' => t('Regular expression for the parent'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the unique 
                           name needed to identify the existing sequence for which the 
                           relationship type selected above will apply.'),
      '#weight' => 6
   ); 
   $form['advanced']['relationship']['parent_type']= array(
      '#type' => 'textfield',
      '#title' => t('Parent Type'),
      '#required' => FALSE,
      '#description' => t('Please enter the Sequence Ontology term for the parent.  For example
                           if the FASTA file being loaded is a set of proteins that are 
                           products of genes, then use the SO term \'gene\' or \'transcript\' or equivalent. However,
                           this type must match the type for already loaded features.'),
      '#weight' => 7
   );

   $form['button'] = array(
      '#type' => 'submit',
      '#value' => t('Import FASTA file'),
      '#weight' => 10,
   );
   return $form;   
}

/**
 *
 *
 * @ingroup fasta_loader
 */
function tripal_feature_fasta_load_form_validate($form, &$form_state){
   $fasta_file = trim($form_state['values']['fasta_file']);
   $organism_id  = $form_state['values']['organism_id'];
   $type         = trim($form_state['values']['seqtype']);
   $method       = trim($form_state['values']['method']);
   $match_type   = trim($form_state['values']['match_type']);
   $library_id   = $form_state['values']['library_id'];
   $re_name      = trim($form_state['values']['re_name']);
   $re_uname     = trim($form_state['values']['re_uname']);
   $re_accession = trim($form_state['values']['re_accession']);
   $db_id        = $form_state['values']['db_id'];
   $rel_type     = $form_state['values']['rel_type'];
   $re_subject   = trim($form_state['values']['re_subject']);
   $parent_type   = trim($form_state['values']['parent_type']);

   if($method == 0){
      $method = 'Insert only';
   }
   if($method == 1){
      $method = 'Update only';
   }
   if($method == 2){
      $method = 'Insert and update';
   }

   if($match_type == 0){
      $match_type = 'Name';
   }

   if($match_type == 1){
      $match_type = 'Unique name';
   }


   if ($re_name and !$re_uname and strcmp($match_type,'Unique name')==0){
      form_set_error('re_uname',t("You must provide a regular expression to identify the sequence unique name"));     
   }

   if (!$re_name and $re_uname and strcmp($match_type,'Name')==0){
      form_set_error('re_name',t("You must provide a regular expression to identify the sequence name"));     
   }

   // check to see if the file is located local to Drupal
   $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $fasta_file; 
   if(!file_exists($dfile)){
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      $dfile = $fasta_file;
   }
   if(!file_exists($dfile)){
      form_set_error('fasta_file',t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
   }

   // make sure if a relationship is specified that all fields are provided.
   if(($rel_type or $parent_type) and !$re_subject){
      form_set_error('re_subject',t("Please provide a regular expression for the parent"));
   }
   if(($rel_type or $re_subject) and !$parent_type){
      form_set_error('parent_type',t("Please provide a SO term for the parent"));
   }
   if(($parent_type or $re_subject) and !$rel_type){
      form_set_error('rel_type',t("Please select a relationship type"));
   }


   // make sure if a database is specified that all fields are provided
   if($db_id and !$re_accession){
      form_set_error('re_accession',t("Please provide a regular expression for the accession"));
   }
   if($re_accession and !$db_id){
      form_set_error('db_id',t("Please select a database"));
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

   // check to make sure the 'relationship' and 'sequence' ontologies are loaded
   $form_state['storage']['dfile'] = $dfile;
}

/**
 *
 *
 * @ingroup fasta_loader
 */
function tripal_feature_fasta_load_form_submit ($form, &$form_state){
   global $user;

   $dfile        = $form_state['storage']['dfile'];
   $organism_id  = $form_state['values']['organism_id'];
   $type         = trim($form_state['values']['seqtype']);
   $method       = trim($form_state['values']['method']);
   $match_type   = trim($form_state['values']['match_type']);
   $library_id   = $form_state['values']['library_id'];
   $re_name      = trim($form_state['values']['re_name']);
   $re_uname     = trim($form_state['values']['re_uname']);
   $re_accession = trim($form_state['values']['re_accession']);
   $db_id        = $form_state['values']['db_id'];
   $rel_type     = $form_state['values']['rel_type'];
   $re_subject   = trim($form_state['values']['re_subject']);
   $parent_type   = trim($form_state['values']['parent_type']);
   $analysis_id = $form_state['values']['analysis_id'];

   if($method == 0){
      $method = 'Insert only';
   }
   if($method == 1){
      $method = 'Update only';
   }
   if($method == 2){
      $method = 'Insert and update';
   }

   if($match_type == 0){
      $match_type = 'Name';
   }

   if($match_type == 1){
      $match_type = 'Unique name';
   }

   $args = array($dfile,$organism_id,$type,$library_id,$re_name,$re_uname,
            $re_accession,$db_id,$rel_type,$re_subject,$parent_type,$method,
            $user->uid,$analysis_id,$match_type);

   tripal_add_job("Import FASTA file: $dfile",'tripal_feature',
      'tripal_feature_load_fasta',$args,$user->uid);
}

/**
 *
 *
 * @ingroup fasta_loader
 */
function tripal_feature_load_fasta($dfile, $organism_id, $type,
   $library_id, $re_name, $re_uname, $re_accession, $db_id, $rel_type,
   $re_subject, $parent_type, $method, $uid, $analysis_id, 
   $match_type,$job = NULL)
{

   print "Opening FASTA file $dfile\n";

    
   $lines = file($dfile,FILE_SKIP_EMPTY_LINES);
   $i = 0;

   $name = '';
   $uname = '';
   $residues = '';
   $num_lines = sizeof($lines);
   $interval = intval($num_lines * 0.01);
   if($interval == 0){
      $interval = 1;
   }

   foreach ($lines as $line_num => $line) {
      $i++;  // update the line count     

      // update the job status every 1% features
      if($job and $i % $interval == 0){
         tripal_job_set_progress($job,intval(($i/$num_lines)*100));
      }

      // if we encounter a definition line then get the name, uniquename, 
      // accession and relationship subject from the definition line
      if(preg_match('/^>/',$line)){
         // if we have a feature name then we are starting a new sequence
         // so let's handle the previous one before moving on
         if($name or $uname){
           tripal_feature_fasta_loader_handle_feature($name,$uname,$db_id,
              $accession,$subject,$rel_type,$parent_type,$analysis_id,$organism_id,$type,
              $source,$residues,$method,$re_name,$match_type);
           $residues = '';
           $name = '';
           $uname = '';
         }

         $line = preg_replace("/^>/",'',$line);
         // get the feature name
         if($re_name){
            if(!preg_match("/$re_name/",$line,$matches)){
               print "WARNING: Regular expression for the feature name finds nothing\n";
            }
            $name = trim($matches[1]);
         } else {
            // if the match_type is name and no regular expression was provided
            // then use the first word as the name, otherwise we don't set the name
            if(strcmp($match_type,'Name')==0){
               preg_match("/^\s*(.*?)[\s\|].*$/",$line,$matches);
               $name = trim($matches[1]);
            }
         } 
         // get the feature unique name
         if($re_uname){
            if(!preg_match("/$re_uname/",$line,$matches)){
               print "WARNING: Regular expression for the feature unique name finds nothing\n";
            }
            $uname = trim($matches[1]);
         } else {
            // if the match_type is name and no regular expression was provided
            // then use the first word as the name, otherwise, we don't set the unqiuename
            if(strcmp($match_type,'Unique name')==0){
               preg_match("/^\s*(.*?)[\s\|].*$/",$line,$matches);
               $uname = trim($matches[1]);
            }
         } 
         // get the accession    
         preg_match("/$re_accession/",$line,$matches);
         $accession = trim($matches[1]);

         // get the relationship subject
         preg_match("/$re_subject/",$line,$matches);
         $subject = trim($matches[1]);
      }
      else {
         $residues .= trim($line);
      }
   }
   // now load the last sequence in the file
   tripal_feature_fasta_loader_handle_feature($name,$uname,$db_id,
      $accession,$subject,$rel_type,$parent_type,$analysis_id,$organism_id,$type,
      $source,$residues,$method,$re_name,$match_type);
   return '';
}

/**
 *
 *
 * @ingroup fasta_loader
 */
function tripal_feature_fasta_loader_handle_feature($name,$uname,$db_id,$accession,
              $parent,$rel_type,$parent_type,$analysis_id,$organism_id,$type, 
              $source,$residues,$method,$re_name,$match_type) 
{
   $previous_db = tripal_db_set_active('chado');

   // first get the type for this sequence
   $cvtermsql = "SELECT CVT.cvterm_id
                 FROM {cvterm} CVT
                    INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
                    LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
                 WHERE cv.name = '%s' and (CVT.name = '%s' or CVTS.synonym = '%s')";
   $cvterm = db_fetch_object(db_query($cvtermsql,'sequence',$type,$type));
   if(!$cvterm){
      print "ERROR: cannot find the term type: '$type'\n";
      return 0;
   }

   // check to see if this feature already exists
   if(strcmp($match_type,'Name')==0){
      $cnt_sql = "SELECT count(*) as cnt FROM {feature} 
                      WHERE organism_id = %d and name = '%s' and type_id = %d";
      $cnt = db_fetch_object(db_query($cnt_sql,$organism_id,$name,$cvterm->cvterm_id));
      if($cnt->cnt > 1){
         print "ERROR: multiple features exist with the name '$name' of type '$type' for the organism.  skipping\n";
         return 0;
      } else {
         $feature_sql = "SELECT * FROM {feature} 
                      WHERE organism_id = %d and name = '%s' and type_id = %d";
         $feature = db_fetch_object(db_query($feature_sql,$organism_id,$name,$cvterm->cvterm_id));
      }
   }
   if(strcmp($match_type,'Unique name')==0){
      $feature_sql = "SELECT * FROM {feature} 
                      WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
      $feature = db_fetch_object(db_query($feature_sql,$organism_id,$uname,$cvterm->cvterm_id));
   }

   if(!$feature and (strcmp($method,'Insert only')==0 or strcmp($method,'Insert and update')==0)){
       // if we have a unique name but not a name then set them to be teh same 
       // and vice versa
       if(!$uname){
          $uname = $name;
       }
       elseif(!$name){
          $name = $uname;
       }
      // now insert the feature
      $sql = "INSERT INTO {feature} 
                 (organism_id, name, uniquename, residues, seqlen, 
                  md5checksum,type_id,is_analysis,is_obsolete)
              VALUES(%d,'%s','%s','%s',%d, '%s', %d, %s, %s)";
      $result = db_query($sql,$organism_id,$name,$uname,$residues,strlen($residues),
                  md5($residues),$cvterm->cvterm_id,'false','false');
      if(!$result){
         print "ERROR: failed to insert feature '$name ($uname)'\n";
         return 0;
      } else {
         print "Inserted feature $name ($uname)\n";
      }
      $feature = db_fetch_object(db_query($feature_sql,$organism_id,$uname,$cvterm->cvterm_id));
   } 
   if(!$feature and (strcmp($method,'Update only')==0 or strcmp($method,'Insert and update')==0)){
      print "WARNING: failed to find feature '$name' ('$uname') while matching on " . strtolower($match_type) . ". Skipping\n";
      return 0;
   }

   if($feature and (strcmp($method,'Update only')==0 or strcmp($method,'Insert and update')==0)){
       if(strcmp($method,'Update only')==0 or strcmp($method,'Insert and update')==0){
         if(strcmp($match_type,'Name')==0){
            // if we're matching on the name but do not have a new unique name then we
            // don't want to update the uniquename.  If we do have a uniquename then we 
            // should update it.  We only get a uniquename if there was a regular expression
            // provided for pulling it out
            if($uname){
               $sql = "UPDATE {feature} 
                        SET uniquename = '%s', residues = '%s', seqlen = '%s', md5checksum = '%s'
                        WHERE organism_id = %d and name = '%s' and type_id = %d";
               $result = db_query($sql,$uname,$residues,strlen($residues),md5($residues),$organism_id,$name,$cvterm->cvterm_id);
            } else {
               $sql = "UPDATE {feature} 
                        SET residues = '%s', seqlen = '%s', md5checksum = '%s'
                        WHERE organism_id = %d and name = '%s' and type_id = %d";
               $result = db_query($sql,$residues,strlen($residues),md5($residues),$organism_id,$name,$cvterm->cvterm_id);
            }
         } else {
            // if we're matching on the unique name but do not have a new name then we
            // don't want to update the name.  If we do have a name then we 
            // should update it.  We only get a name if there was a regular expression
            // provided for pulling it out
            if($name){
               $sql = "UPDATE {feature} 
                        SET name = '%s', residues = '%s', seqlen = '%s', md5checksum = '%s'
                        WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
               $result = db_query($sql,$name,$residues,strlen($residues),md5($residues),$organism_id,$uname,$cvterm->cvterm_id);
            } else {
               $sql = "UPDATE {feature} 
                        SET residues = '%s', seqlen = '%s', md5checksum = '%s'
                        WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
               $result = db_query($sql,$residues,strlen($residues),md5($residues),$organism_id,$uname,$cvterm->cvterm_id);
            }
         }
         if(!$result){
            print "ERROR: failed to update feature '$name ($uname)'\n";
            return 0;
         } else {
            print "Updated feature $name ($uname)\n";
         }
      } else {
         print "WARNING: feature already exists: '$name' ('$uname'). Skipping\n";
      }
   }
   // now get the feature
   $feature = db_fetch_object(db_query($feature_sql,$organism_id,$uname,$cvterm->cvterm_id));
   if(!$feature){
      print "Something bad has happened: $organism_id, $uname, $cvterm->cvterm_id\n";
      return 0;
   }

	 // add in the analysis link
	 if ($analysis_id) {
	 	$analysis_link_sql = 'SELECT * FROM analysisfeature WHERE analysis_id=%d AND feature_id=%d';
	 	$analysis_link = db_fetch_object(db_query($analysis_link_sql, $analysis_id, $feature->feature_id));
	 	if (!$analysis_link) {
	 		$sql = "INSERT INTO analysisfeature (analysis_id, feature_id) VALUES (%d, %d)";
	 		$result = db_query($sql, $analysis_id, $feature->feature_id);
		  if(!$result){
			  print "WARNING: could not add link between analysis: ".$analysis_id." and feature: ".$feature->uniquename."\n";
		  }
		  $analysis_link = db_fetch_object(db_query($analysis_link_sql, $analysis_id, $feature->feature_id));
	 	}
	 }
	 
   // now add the database cross reference
   if($db_id){
      // check to see if this accession reference exists, if not add it
      $dbxrefsql = "SELECT * FROM {dbxref} WHERE db_id = %s and accession = '%s'";
      $dbxref = db_fetch_object(db_query($dbxrefsql,$db_id,$accession));
      if(!$dbxref){
         $sql = "INSERT INTO {dbxref} (db_id,accession) VALUES (%d,'%s')";
         $result = db_query($sql,$db_id,$accession);
         if(!$result){
           print "WARNING: could not add external database acession: '$name accession: $accession'\n";
         }
         $dbxref = db_fetch_object(db_query($dbxrefsql,$db_id,$accession));
      }

      // check to see if the feature dbxref record exists if not, then add it 
      $fdbxrefsql = "SELECT * FROM {feature_dbxref} WHERE feature_id = %d and dbxref_id = %d";
      $fdbxref = db_fetch_object(db_query($fdbxrefsql,$feature->feature_id,$dbxref->dbxref_id));
      if(!$fdbxref){
         $sql = "INSERT INTO {feature_dbxref} (feature_id,dbxref_id) VALUES (%d,%d)";
         $result = db_query($sql,$feature->feature_id,$dbxref->dbxref_id);
         if(!$result){
            print "WARNING: could not associate database cross reference with feature: '$name accession: $accession'\n";
         } else {
            print "Added database crossreference $name ($uname) -> $accession\n";
         }
      }
   }

   // now add in the relationship if one exists.  First, get the parent type for the relationship
   // then get the parent feature 
   if($rel_type){
      $parentcvterm = db_fetch_object(db_query($cvtermsql,'sequence',$parent_type,$parent_type));
      $relcvterm = db_fetch_object(db_query($cvtermsql,'relationship',$rel_type,$rel_type));
      $parent_feature = db_fetch_object(db_query($feature_sql,$organism_id,$parent,$parentcvterm->cvterm_id));
      if($parent_feature){
         // check to see if the relationship already exists
         $sql = "SELECT * FROM {feature_relationship} WHERE subject_id = %d and object_id = %d and type_id = %d";
         $rel = db_fetch_object(db_query($sql,$feature->feature_id,$parent_feature->feature_id,$relcvterm->cvterm_id));
         if($rel){
            print "WARNING: relationship already exists, skipping '$uname' ($type) $rel_type '$parent' ($parent_type)\n";
         } else {      
            $sql = "INSERT INTO {feature_relationship} (subject_id,object_id,type_id)
                    VALUES (%d,%d,%d)";
            $result = db_query($sql,$feature->feature_id,$parent_feature->feature_id,$relcvterm->cvterm_id);
            if(!$result){
               print "WARNING: failed to insert feature relationship '$uname' ($type) $rel_type '$parent' ($parent_type)\n";
            } else {
               print "Inserted relationship relationship: '$uname' ($type) $rel_type '$parent' ($parent_type)\n";
            }
         } 
      }
      else {
         print "WARNING: cannot establish relationship '$uname' ($type) $rel_type '$parent' ($parent_type): Cannot find the parent\n";
      }
   }
   tripal_db_set_active($previous_db);
}

