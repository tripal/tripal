<?php

function tripal_feature_fasta_load_form (){

   $form['fasta_file']= array(
      '#type'          => 'textfield',
      '#title'         => t('FASTA File'),
      '#description'   => t('Please enter the full system path for the FASTA file, or a path within the Drupal
                             installation (e.g. /sites/default/files/xyz.obo).  The path must be accessible to the
                             server on which this Drupal instance is running.'),
      '#required' => TRUE,
      '#weight'        => 1
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
     '#weight'      => 2,
   );
   $form['type']= array(
      '#type' => 'textfield',
      '#title' => t('Sequence Type'),
      '#required' => TRUE,
      '#description' => t('Please enter the Sequence Ontology term that describes the sequences in the FASTA file.'),
      '#weight' => 3
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
   $form['update']= array(
      '#type' => 'checkbox',
      '#title' => t('Insert and update'),
      '#required' => FALSE,
      '#description' => t('By default only new features are inserted.  Select this checkbox to update
                           features that already exists with the contents from the FASTA file.'),
      '#weight' => 6
   );

   // Advanced Options
   $form['advanced'] = array(
      '#type' => 'fieldset',
      '#title' => t('Advanced Options'),
      '#weight'=> 7,
      '#collapsed' => TRUE
   );
   $form['advanced']['re_help']= array(
      '#type' => 'item',
      '#value' => t('A regular expression is an advanced method for extracting information from a string of text.  
                     By default, this loader will use the first word in the definition line for each sequence in the FASTA file
                     as the uniquename for the sequences.  If this is not desired, you may use the following regular 
                     expressions to define the postions of the unique name.'),
      '#weight' => 0
   );
   $form['advanced']['re_name']= array(
      '#type' => 'textfield',
      '#title' => t('Regular expression for the name'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the feature name from the FASTA definition line. For example, for a defintion line with a name and uniquename separated by a bar \'|\' (>seqname|uniquename), the regular expression would be, "^(.*?)\|.*$"'),
      '#weight' => 1
   );   
   $form['advanced']['re_uname']= array(
      '#type' => 'textfield',
      '#title' => t('Regular expression for the unique name'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the unique feature name for each feature from the FASTA definition line.  This name must be unique for the organism.'),
      '#weight' => 2
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
/*************************************************************************
*
*/
function tripal_feature_fasta_load_form_validate($form, &$form_state){
   $fasta_file = trim($form_state['values']['fasta_file']);
   $organism_id  = $form_state['values']['organism_id'];
   $type         = trim($form_state['values']['type']);
   $update       = trim($form_state['values']['update']);
   $library_id   = $form_state['values']['library_id'];
   $re_name      = trim($form_state['values']['re_name']);
   $re_uname     = trim($form_state['values']['re_uname']);
   $re_accession = trim($form_state['values']['re_accession']);
   $db_id        = $form_state['values']['db_id'];
   $rel_type     = $form_state['values']['rel_type'];
   $re_subject   = trim($form_state['values']['re_subject']);
   $parent_type   = trim($form_state['values']['parent_type']);

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
/*************************************************************************
*
*/
function tripal_feature_fasta_load_form_submit ($form, &$form_state){
   global $user;

   $dfile        = $form_state['storage']['dfile'];
   $organism_id  = $form_state['values']['organism_id'];
   $type         = trim($form_state['values']['type']);
   $update       = trim($form_state['values']['update']);
   $library_id   = $form_state['values']['library_id'];
   $re_name      = trim($form_state['values']['re_name']);
   $re_uname     = trim($form_state['values']['re_uname']);
   $re_accession = trim($form_state['values']['re_accession']);
   $db_id        = $form_state['values']['db_id'];
   $rel_type     = $form_state['values']['rel_type'];
   $re_subject   = trim($form_state['values']['re_subject']);
   $parent_type   = trim($form_state['values']['parent_type']);

   $args = array($dfile,$organism_id,$type,$library_id,$re_name,$re_uname,
            $re_accession,$db_id,$rel_type,$re_subject,$parent_type,$update,$user->uid);

   tripal_add_job("Import FASTA file: $dfile",'tripal_core',
      'tripal_feature_load_fasta',$args,$user->uid);
}
/*************************************************************************
*
*/
function tripal_feature_load_fasta($dfile, $organism_id, $type,
   $library_id, $re_name, $re_uname, $re_accession, $db_id, $rel_type,
   $re_subject, $parent_type, $update,$uid, $job = NULL)
{

   print "Opening FASTA file $dfile\n";

    
   $lines = file($dfile,FILE_SKIP_EMPTY_LINES);
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

      // update the job status every 1% features
      if($job and $i % $interval == 0){
         tripal_job_set_progress($job,intval(($i/$num_lines)*100));
      }

      // get the name, uniquename, accession and relationship subject from
      // the definition line
      if(preg_match('/^>/',$line)){
         // if we have a feature name then we are starting a new sequence
         // and we need to insert this one
         if($name){
           tripal_feature_fasta_loader_insert_feature($name,$uname,$db_id,
              $accession,$subject,$rel_type,$parent_type,$library_id,$organism_id,$type,
              $source,$residues,$update);
           $residues = '';
           $name = '';
         }

         $line = preg_replace("/^>/",'',$line);
         if($re_name){
            if(!preg_match("/$re_name/",$line,$matches)){
               print "Regular expression for the feature name finds nothing\n";
            }
            $name = trim($matches[1]);
         } else {
            preg_match("/^(.*?)[\s\|].*$/",$line,$matches);
            $name = trim($matches[1]);
         }
         if($re_uname){
            preg_match("/$re_uname/",$line,$matches);
            $uname = trim($matches[1]);
         } else {
            preg_match("/^(.*?)[\s\|].*$/",$line,$matches);
            $uname = trim($matches[1]);
         }         
         preg_match("/$re_accession/",$line,$matches);
         $accession = trim($matches[1]);
         preg_match("/$re_subject/",$line,$matches);
         $subject = trim($matches[1]);
//         print "Name: $name, UName: $uname, Accession: $accession, Subject: $subject\n";
      }
      else {
         $residues .= trim($line);
      }
   }
   // now load the last sequence in the file
   tripal_feature_fasta_loader_insert_feature($name,$uname,$db_id,
      $accession,$subject,$rel_type,$parent_type,$library_id,$organism_id,$type,
      $source,$residues,$update,$re_name);
   return '';
}
/*************************************************************************
*
*/
function tripal_feature_fasta_loader_insert_feature($name,$uname,$db_id,$accession,
              $parent,$rel_type,$parent_type,$library_id,$organism_id,$type, 
              $source,$residues,$update,$re_name) 
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
   $feature_sql = "SELECT * FROM {feature} 
                   WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
   $feature = db_fetch_object(db_query($feature_sql,$organism_id,$uname,$cvterm->cvterm_id));
   if(!$feature){
      // now insert the feature
      $sql = "INSERT INTO {feature} (organism_id, name, uniquename, residues, seqlen, md5checksum,type_id,is_analysis,is_obsolete)
              VALUES(%d,'%s','%s','%s',%d, '%s', %d, %s, %s)";
      $result = db_query($sql,$organism_id,$name,$uname,$residues,strlen($residues),
                  md5($residues),$cvterm->cvterm_id,'false','false');
      if(!$result){
         print "ERROR: failed to insert feature '$name ($uname)'\n";
         return 0;
      } else {
         print "Inserted feature $name ($uname)\n";
      }
   } else {
       if($update){

         // we do not want to wipe out the name if the user did not intend for this to
         // happen.  The uniquename must match the sequence but the name may not.  
         // so, we'll only update the name if the users specified an 're_name' regular
         // expression.
         if($re_name){
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
         if(!$result){
            print "ERROR: failed to update feature '$name ($uname)'\n";
            return 0;
         } else {
            print "Updated feature $name ($uname)\n";
         }
      } else {
         print "WARNING: feature already exists, skipping: '$name ($uname)'\n";
      }
   }
   // now get the feature
   $feature = db_fetch_object(db_query($feature_sql,$organism_id,$uname,$cvterm->cvterm_id));
   if(!$feature){
      print "Something bad has happened: $organism_id, $uname, $cvterm->cvterm_id\n";
      return 0;
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

