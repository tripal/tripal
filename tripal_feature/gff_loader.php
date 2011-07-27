<?php

/**
 * @defgroup gff3_loader GFF3 Feature Loader
 * @{
 * Provides gff3 loading functionality. Creates features based on their specification in a GFF3 file.
 * @}
 * @ingroup tripal_feature
 */
// TODO: The rank column on the feature_relationship table needs to be used to
//       make sure the ordering of CDS (exons) is correct.

// The entries in the GFF file are not in order so the order of the relationships
// is not put in correctly.
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_gff3_load_form (){

   $form['gff_file']= array(
      '#type'          => 'textfield',
      '#title'         => t('GFF3 File'),
      '#description'   => t('Please enter the full system path for the GFF file, or a path within the Drupal
                             installation (e.g. /sites/default/files/xyz.gff).  The path must be accessible to the
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
   );
   $form['import_options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Import Options'),
      '#weight'=> 6,
      '#collapsed' => TRUE
   );
   $form['import_options']['add_only']= array(
      '#type' => 'checkbox',
      '#title' => t('Import only new features'),
      '#required' => FALSE,
      '#description' => t('The job will skip features in the GFF file that already
                           exist in the database and import only new features.'),
      '#weight' => 2
   );
   $form['import_options']['update']= array(
      '#type' => 'checkbox',
      '#title' => t('Import all and update'),
      '#required' => FALSE,
      '#default_value' => 'checked',
      '#description' => t('Existing features will be updated and new features will be added.  Attributes 
                           for a feature that are not present in the GFF but which are present in the 
                           database will not be altered.'),
      '#weight' => 3
   );
   $form['import_options']['refresh']= array(
      '#type' => 'checkbox',
      '#title' => t('Import all and replace'),
      '#required' => FALSE,
      '#description' => t('Existing features will be updated and feature properties not
                           present in the GFF file will be removed.'),
      '#weight' => 4
   );
   $form['import_options']['remove']= array(
      '#type' => 'checkbox',
      '#title' => t('Delete features'),
      '#required' => FALSE,
      '#description' => t('Features present in the GFF file that exist in the database
                           will be removed rather than imported'),
      '#weight' => 5
   );

   $form['analysis'] = array(
      '#type' => 'fieldset',
      '#title' => t('Analysis Used to Derive Features'),
      '#weight'=> 6,
      '#collapsed' => TRUE
   ); 
   $form['analysis']['desc'] = array(
      '#type' => 'markup',
      '#value' => t("Why specify an analysis for a data load?  All data comes 
         from some place, even if downloaded from Genbank. By specifying
         analysis details for all data uploads, it allows an end user to reproduce the
         data set, but at least indicates the source of the data."), 
   );

   // get the list of analyses
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

   $form['button'] = array(
      '#type' => 'submit',
      '#value' => t('Import GFF3 file'),
      '#weight' => 10,
   );


   return $form;
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_gff3_load_form_validate ($form, &$form_state){

   $gff_file = $form_state['values']['gff_file'];
   $organism_id = $form_state['values']['organism_id'];
   $add_only = $form_state['values']['add_only'];
   $update   = $form_state['values']['update'];
   $refresh  = $form_state['values']['refresh'];
   $remove   = $form_state['values']['remove'];

   // check to see if the file is located local to Drupal
   $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $gff_file; 
   if(!file_exists($dfile)){
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      $dfile = $gff_file;
   }
   if(!file_exists($dfile)){
      form_set_error('gff_file',t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
   }

   if (($add_only and ($update or $refresh or $remove)) or 
       ($update   and ($add_only or $refresh or $remove)) or
       ($refresh and ($update or $add_only or $remove)) or
       ($remove and ($update or $refresh or $add_only))){
       form_set_error('add_only',t("Please select only one checkbox from the import options section"));
   }
}

/**
 *
 * @ingroup gff3_loader
 */
function tripal_feature_gff3_load_form_submit ($form, &$form_state){
   global $user;

   $gff_file = $form_state['values']['gff_file'];
   $organism_id = $form_state['values']['organism_id'];
   $add_only = $form_state['values']['add_only'];
   $update   = $form_state['values']['update'];
   $refresh  = $form_state['values']['refresh'];
   $remove   = $form_state['values']['remove'];
   $analysis_id = $form_state['values']['analysis_id'];

   $args = array($gff_file,$organism_id,$analysis_id,$add_only,$update,$refresh,$remove);
   $type = '';
   if($add_only){
     $type = 'import only new features';
   }
   if($update){
     $type = 'import all and update';
   }
   if($refresh){
     $type = 'import all and replace';
   }
   if($remove){
     $type = 'delete features';
   }
   tripal_add_job("$type GFF3 file $gff_file",'tripal_feature',
      'tripal_feature_load_gff3',$args,$user->uid);

   return '';
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3($gff_file, $organism_id,$analysis_id,$add_only =0, 
   $update = 0, $refresh = 0, $remove = 0, $job = NULL)
{

   // this array is used to cache all of the features in the GFF file and
   // used to lookup parent and target relationships
   $gff_features = array();
 
   // check to see if the file is located local to Drupal
   $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $gff_file; 
   if(!file_exists($dfile)){
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      $dfile = $gff_file;
   }
   if(!file_exists($dfile)){
      print "ERROR: cannot find the file: $dfile\n";
      return 0;
   }

   $previous_db = tripal_db_set_active('chado');
   print "Opening $gff_file\n";
    
   //$lines = file($dfile,FILE_SKIP_EMPTY_LINES);
   $fh = fopen($dfile,'r');
   if(!$fh){
      print "ERROR: cannot open file: $dfile\n";
      return 0;
   }
   $filesize = filesize($dfile);

   // get the controlled vocaubulary that we'll be using.  The
   // default is the 'sequence' ontology
   $sql = "SELECT * FROM cv WHERE name = '%s'";
   $cv = db_fetch_object(db_query($sql,'sequence'));
   if(!$cv){
      print "ERROR:  cannot find the 'sequence' ontology\n";
      return '';
   }

   // get the organism for which this GFF3 file belongs
   $sql = "SELECT * FROM organism WHERE organism_id = %d";
   $organism = db_fetch_object(db_query($sql,$organism_id));

   $interval = intval($filesize * 0.01);
   if($interval == 0){
      $interval = 1;
   }
   $in_fasta = 0;
//   foreach ($lines as $line_num => $line) {
   $line_num = 0;
   $num_read = 0;

   while($line = fgets($fh)){

      $line_num++;
      $num_read += strlen($line);

      // update the job status every 1% features
      if($job and $num_read % $interval == 0){
         tripal_job_set_progress($job,intval(($num_read/$filesize)*100));
      }
      // check to see if we have FASTA section, if so then set the variable
      // to start parsing
      if(preg_match('/^##FASTA/i',$line)){
         $in_fasta = 1;
         break;
      }
      // skip comments
      if(preg_match('/^#/',$line)){
         continue; 
      }
      // skip empty lines
      if(preg_match('/^\s*$/',$line)){
         continue; 
      }
      

      // handle FASTA section
      
      // TODO: handle URL encoding

      // remove URL encoding and get the columns
      $cols = explode("\t",$line);
      if(sizeof($cols) != 9){
         print "ERROR: improper number of columns on line $line_num\n";
         print_r($cols);
         return '';
      }
      // get the column values
      $landmark = $cols[0];
      $source = $cols[1];
      $type = $cols[2];
      $start = $cols[3];    
      $end = $cols[4];
      $score = $cols[5];
      $strand = $cols[6];
      $phase = $cols[7];
      $attrs = explode(";",$cols[8]);  // split by a semi-colon

      // ready the start and stop for chado.  Chado expects these positions
      // to be zero-based, so we substract 1 from the fmin
      $fmin = $start - 1;
      $fmax = $end;
      if($end < $start){
         $fmin = $end - 1;
         $fmax = $start;
      }
      // format the strand for chado
      if(strcmp($strand,'.')==0){
         $strand = 0;
      }
      elseif(strcmp($strand,'+')==0){
         $strand = 1;
      }
      elseif(strcmp($strand,'-')==0){
         $strand = -1;
      }
      if(strcmp($phase,'.')==0){
         $phase = '';
      }

      // get the type record
      $cvtermsql = "SELECT CVT.cvterm_id, CVT.cv_id, CVT.name, CVT.definition,
                       CVT.dbxref_id, CVT.is_obsolete, CVT.is_relationshiptype
                    FROM {cvterm} CVT
                       INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
                       LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
                    WHERE CV.cv_id = %d and (CVT.name = '%s' or CVTS.synonym = '%s')";
      $cvterm = db_fetch_object(db_query($cvtermsql,$cv->cv_id,$type,$type));
      if(!$cvterm){
         print "ERROR: cannot find ontology term '$type' on line $line_num.\n";
         return '';
      }

      // break apart each of the attributes
      $tags = array();
      $attr_name = '';
      $attr_uniquename = '';
      $attr_residue_info = '';
      $attr_locgroup = 0;
      $attr_fmin_partial = 'f';
      $attr_fmax_partial = 'f';
      $attr_is_obsolete = 'f';
      $attr_is_analysis = 'f';
      $attr_others = '';
      $residues = '';
      foreach($attrs as $attr){
         $attr = rtrim($attr);
         $attr = ltrim($attr);
         if(strcmp($attr,'')==0){
            continue;
         }
         if(!preg_match('/^[^\=]+\=[^\=]+$/',$attr)){
            print "ERROR: attribute is not correctly formatted on line $line_num: $attr\n";
            return '';
         }

         // break apart each tag
         $tag = explode("=",$attr);  // split by equals sign
         // multiple instances of an attribute are separated by commas
         $tags[$tag[0]] = explode(",",$tag[1]);  // split by comma
         if(strcmp($tag[0],'ID')==0){
            $attr_uniquename = $tag[1];
         }
         elseif(strcmp($tag[0],'Name')==0){
            $attr_name = $tag[1];
         }
         // get the list of other attributes other than those reserved ones.
         elseif(strcmp($tag[0],'Alias')!=0        and strcmp($tag[0],'Parent')!=0 and 
                strcmp($tag[0],'Target')!=0       and strcmp($tag[0],'Gap')!=0 and
                strcmp($tag[0],'Derives_from')!=0 and strcmp($tag[0],'Note')!=0 and
                strcmp($tag[0],'Dbxref')!=0       and strcmp($tag[0],'Ontology_term')!=0 and
                strcmp($tag[0],'Is_circular')!=0){
            $attr_others[$tag[0]] = $tag[1];
         }
      }

      // if neither name nor uniquename are provided then generate one
      if(!$attr_uniquename and !$attr_name){
         if(array_key_exists('Parent',$tags)){
            $attr_uniquename = $tags['Parent'][0]."-$type-$landmark:$fmin..$fmax";
         } else { 
           print "ERROR: cannot generate a uniquename for feature on line $line_num\n";
           exit;
         }
         $attr_name = $attr_uniquename;
      }

      // if a name is not specified then use the unique name
      if(strcmp($attr_name,'')==0){
         $attr_name = $attr_uniquename;
      }

      // if an ID attribute is not specified then use the attribute name and
      // hope for the best
      if(!$attr_uniquename){
         $attr_uniquename = $attr_name;
      }

      // make sure the landmark sequence exists in the database.  We don't
      // know the type of the landmark so we'll hope that it's unique across
      // all types. If not we'll error out.  This test is only necessary if
      // if the landmark and the uniquename are different.  If they are the same
      // then this is the information for the landmark
      if(strcmp($landmark,$attr_uniquename)!=0){
         $feature_sql = "SELECT count(*) as num_landmarks
                         FROM {feature} 
                         WHERE organism_id = %d and uniquename = '%s'";
         $count = db_fetch_object(db_query($feature_sql,$organism_id,$landmark));
         if(!$count or $count->num_landmarks == 0){
            print "ERROR: the landmark '$landmark' cannot be found for this organism. ". 
                  "Please add the landmark and then retry the import of this GFF3 ".
                  "file.\n";
            return '';

         }
         if($count->num_landmarks > 1){
            print "ERROR: the landmark '$landmark' is not unique for this organism. ".
                  "The features cannot be associated.\n";
            return '';
         }
      }
      
      // if the option is to remove or refresh then we want to remove
      // the feature from the database.
      if($remove or $refresh){
         print "Removing feature '$attr_uniquename'\n";
         $sql = "DELETE FROM {feature}
                 WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
         $result = db_query($sql,$organism->organism_id,$attr_uniquename,$cvterm->cvterm_id); 
         if(!$result){
            print "ERROR: cannot delete feature $attr_uniquename\n";
         }
         $feature = 0; 
      }

      // add or update the feature and all properties
      if($update or $refresh or $add_only){
    

         // add/update the feature
         print "line $line_num, ". intval(($num_read/$filesize)*100). "%. ";
         $feature = tripal_feature_load_gff3_feature($organism,$analysis_id,$cvterm,
            $attr_uniquename,$attr_name,$residues,$attr_is_analysis,
            $attr_is_obsolete, $add_only,$score);

         // store all of the features for use later by parent and target
         // relationships
         $gff_features[$feature->uniquename]['type'] = $type;
         $gff_features[$feature->uniquename]['strand'] = $strand;

         if($feature){

            // add/update the featureloc if the landmark and the ID are not the same
            // if they are the same then this entry in the GFF is probably a landmark identifier
            if(strcmp($landmark,$attr_uniquename)!=0){
               tripal_feature_load_gff3_featureloc($feature,$organism,
                  $landmark,$fmin,$fmax,$strand,$phase,$attr_fmin_partial,
                  $attr_fmax_partial,$attr_residue_info,$attr_locgroup);
            }
            // add any aliases for this feature
            if(array_key_exists('Alias',$tags)){
               tripal_feature_load_gff3_alias($feature,$tags['Alias']);
            }
            // add any dbxrefs for this feature
            if(array_key_exists('Dbxref',$tags)){
               tripal_feature_load_gff3_dbxref($feature,$tags['Dbxref']);
            }
            // add any ontology terms for this feature
            if(array_key_exists('Ontology_term',$tags)){
               tripal_feature_load_gff3_ontology($feature,$tags['Ontology_term']);
            }
            // add parent relationships
            if(array_key_exists('Parent',$tags)){
               tripal_feature_load_gff3_parents($feature,$cvterm,$tags['Parent'],$gff_features,$organism_id,$fmin);
            }
            // add target relationships
            if(array_key_exists('Target',$tags)){
               $target = explode(" ",$tags['Target'][0]);
               $target_feature = $target[0];
               $target_start = $target[1];
               $target_end = $target[2];
               $target_dir = $target[3];
               #print "Target: $target_feature, $target_start-$target_end\n";
               tripal_feature_load_gff3_featureloc($feature,$organism,
                  $target_feature,$target_start,$target_end,$strand,$phase,$attr_fmin_partial,
                  $attr_fmax_partial,$attr_residue_info,$attr_locgroup);
            }
            // add gap information.  This goes in simply as a property 
            if(array_key_exists('Gap',$tags)){
               tripal_feature_load_gff3_property($feature,'Gap',$tags['Gap'][0]);
            }
            // add notes. This goes in simply as a property
            if(array_key_exists('Note',$tags)){
               tripal_feature_load_gff3_property($feature,'Note',$tags['Note'][0]);
            }
            // add the Derives_from relationship (e.g. polycistronic genes).
            if(array_key_exists('Derives_from',$tags)){
               tripal_feature_load_gff3_derives_from($feature,$tags['Derives_from'][0],$gff_features,$organism);
            }

            // add in the GFF3_source dbxref so that GBrowse can find the feature using the source column
            $source_ref = array('GFF_source:'.$source);
            tripal_feature_load_gff3_dbxref($feature,$source_ref);

            // add any additional attributes
            if($attr_others){
               foreach($attr_others as $property => $value){
                  tripal_feature_load_gff3_property($feature,$property,$value);
               }
            }
         }
      } 
   }
   // now set the rank of any parent/child relationships.  The order is based
   // on the fmin.  The start rank is 1.  This allows features with other
   // relationships to be '0' (the default), and doesn't interfer with the
   // ordering defined here.
   foreach($gff_features as $parent => $details){
      // only iterate through parents that have children

      if($details['children']){
         // get the parent
         $values = array(
            'uniquename' => $parent,
            'type_id' => array(
               'cv_id' => array(
                  'name' => 'sequence'
               ),
               'name' => $details['type'],
            ),
            'organism_id' => $organism->organism_id,
         );
         $pfeature = tripal_core_chado_select('feature',array('*'),$values);

         // sort the children by order of their fmin positions (values of assoc. array)
         // if the parent is on the reverse strand then sort in reverse
         if($details['strand'] == -1){
            arsort($details['children']); 
         } else {
            asort($details['children']); 
         }

         // now iterate through the children and set their rank
         $rank = 1;
         print "Updating child ranks for $parent (".$details['type'].")\n";
         foreach($details['children'] as $kfeature_id => $kfmin){
            $match = array(
               'object_id' => $pfeature[0]->feature_id,
               'subject_id' => $kfeature_id,
               'type_id' => array(
                  'cv_id' => array(
                     'name' => 'relationship'
                  ),
                  'name' => 'part_of',
               ),
            );
            $values = array(
               'rank' => $rank,          
            );
            tripal_core_chado_update('feature_relationship',$match,$values);
            $rank++;
         }
      }
   }

   tripal_db_set_active($previous_db);
   return '';
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_derives_from($feature,$subject,$gff_features,$organism){

   // first get the subject feature
   $match = array(
      'organism_id' => $organism->organism_id,
      'uniquename' => $subject,
      'type_id' => array(
         'name' => $gff_features[$subject]['type'],
         'cv_id' => array(
            'name' => 'sequence'
         ),
      ),
   );
   $sfeature = tripal_core_chado_select('feature',array('*'),$match);
   if(count($sfeature)==0){
      print "ERROR: could not add 'Derives_from' relationship for $feature->uniquename and $subject.  Subject feature, '$subject', cannot be found\n";
      return;
   }

   // now check to see if the relationship already exists  
   $values = array(
      'object_id' => $sfeature[0]->feature_id,
      'subject_id' => $feature->feature_id,
      'type_id' => array(
         'cv_id' => array(
            'name' => 'relationship'
         ),
         'name' => 'derives_from',
      ),
      'rank' => 0
   );
   $rel = tripal_core_chado_select('feature_relationship',array('*'),$values);
   if(count($rel) > 0){
      print "   Relationship already exists: $feature->uniquename derives_from $subject\n";
      return;
   }

   // finally insert the relationship if it doesn't exist
   $ret = tripal_core_chado_insert('feature_relationship',$values);
   if(!$ret){
      print "ERROR: could not add 'Derives_from' relationship for $feature->uniquename and $subject\n";
   } else {
      print "   Added relationship: $feature->uniquename derives_from $subject\n";
   }
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_parents($feature,$cvterm,$parents,&$gff_features,$organism_id,$fmin){

   $uname = $feature->uniquename;
   $type = $cvterm->name;
   $rel_type = 'part_of';


   // create these SQL statements that will be used repeatedly below.
   $cvtermsql = "SELECT CVT.cvterm_id
                 FROM {cvterm} CVT
                    INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
                    LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
                 WHERE cv.name = '%s' and (CVT.name = '%s' or CVTS.synonym = '%s')";

   $feature_sql = "SELECT * FROM {feature} 
                   WHERE organism_id = %d and uniquename = '%s' and type_id = %d";

   // iterate through the parents in the list
   foreach($parents as $parent){  
      $parent_type = $gff_features[$parent]['type'];

      // try to find the parent
      $parentcvterm = db_fetch_object(db_query($cvtermsql,'sequence',$parent_type,$parent_type));
      $relcvterm = db_fetch_object(db_query($cvtermsql,'relationship',$rel_type,$rel_type));
      $parent_feature = db_fetch_object(db_query($feature_sql,$organism_id,$parent,$parentcvterm->cvterm_id));

      // we want to add this feature to the child list for the parent
      // when the loader finishes, it will go back through the parent
      // features and rank the children by position
      $gff_features[$parent]['children'][$feature->feature_id] = $fmin;

      // if the parent exists then add the relationship otherwise print error and skip
      if($parent_feature){

         // check to see if the relationship already exists
         $sql = "SELECT * FROM {feature_relationship} WHERE subject_id = %d and object_id = %d and type_id = %d";
         $rel = db_fetch_object(db_query($sql,$feature->feature_id,$parent_feature->feature_id,$relcvterm->cvterm_id));
         if($rel){
            print "   Relationship already exists, skipping '$uname' ($type) $rel_type '$parent' ($parent_type)\n";
         } else {  
            // the relationship doesn't already exist, so add it.    
            $sql = "INSERT INTO {feature_relationship} (subject_id,object_id,type_id)
                    VALUES (%d,%d,%d)";
            $result = db_query($sql,$feature->feature_id,$parent_feature->feature_id,$relcvterm->cvterm_id);
            if(!$result){
               print "WARNING: failed to insert feature relationship '$uname' ($type) $rel_type '$parent' ($parent_type)\n";
            } else {
               print "   Inserted relationship relationship: '$uname' ($type) $rel_type '$parent' ($parent_type)\n";
            }
         }          
      }
      else {
         print "WARNING: cannot establish relationship '$uname' ($type) $rel_type '$parent' ($parent_type): Cannot find the parent\n";
      }
   }
}

/**
 *
 *
 * @ingroup gff3_loader
 */

function tripal_feature_load_gff3_dbxref($feature,$dbxrefs){

   // iterate through each of the dbxrefs
   foreach($dbxrefs as $dbxref){

      // get the database name from the reference.  If it doesn't exist then create one.
      $ref = explode(":",$dbxref);
      $dbname = $ref[0];
      $accession = $ref[1];

      // first look for the database name if it doesn't exist then create one.
      // first check for the fully qualified URI (e.g. DB:<dbname>. If that
      // can't be found then look for the name as is.  If it still can't be found
      // the create the database
      $db = tripal_core_chado_select('db',array('db_id'),array('name' => "DB:$dbname"));  
      if(sizeof($db) == 0){
         $db = tripal_core_chado_select('db',array('db_id'),array('name' => "$dbname"));      
      }        
      if(sizeof($db) == 0){
         $ret = tripal_core_chado_insert('db',array('name' => $dbname, 
           'description' => 'Added automatically by the GFF loader'));
         if($ret){ 
            print "   Added new database: $dbname\n";
            $db = tripal_core_chado_select('db',array('db_id'),array('name' => "$dbname"));      
         } else {
            print "ERROR: cannot find or add the database $dbname\n";
            return 0;
         }
      } 
      $db = $db[0];
       
      // now check to see if the accession exists
      $dbxref = tripal_core_chado_select('dbxref',array('dbxref_id'),array(
         'accession' => $accession,'db_id' => $db->db_id));

      // if the accession doesn't exist then we want to add it
      if(sizeof($dbxref) == 0){
         $ret = tripal_core_chado_insert('dbxref',array('db_id' => $db->db_id,
            'accession' => $accession,'version' => ''));
         $dbxref = tripal_core_chado_select('dbxref',array('dbxref_id'),array(
            'accession' => $accession,'db_id' => $db->db_id));
      }
      $dbxref = $dbxref[0];

      // check to see if this feature dbxref already exists
      $fdbx = tripal_core_chado_select('feature_dbxref',array('feature_dbxref_id'),
         array('dbxref_id' => $dbxref->dbxref_id,'feature_id' => $feature->feature_id));

      // now associate this feature with the database reference if it doesn't
      // already exist
      if(sizeof($fdbx)==0){
         $ret = tripal_core_chado_insert('feature_dbxref',array(
            'feature_id' => $feature->feature_id,
            'dbxref_id' => $dbxref->dbxref_id));
         if($ret){
            print "   Adding Dbxref $dbname:$accession\n";
         } else {
            print "ERROR: failed to insert Dbxref: $dbname:$accession\n";
            return 0;
         }
      } else {
         print "   Dbxref already associated, skipping $dbname:$accession\n";
      }
   }
   return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_ontology($feature,$dbxrefs){

   // iterate through each of the dbxrefs
   foreach($dbxrefs as $dbxref){

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
            print "   Adding ontology term $dbname:$accession\n";
         } else {
            print "ERROR: failed to insert ontology term: $dbname:$accession\n";
            return 0;
         }
      } else {
         print "   Ontology term already associated, skipping $dbname:$accession\n";
      }
   }
   return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_alias($feature,$aliases){

   // make sure we have a 'synonym_type' vocabulary
   $sql = "SELECT * FROM {cv} WHERE name='synonym_type'";
   $syncv = db_fetch_object(db_query($sql));
   if(!$syncv){
      $sql = "INSERT INTO {cv} (name,definition) VALUES ('synonym_type','')";
      if(!db_query($sql)){
         print("ERROR: Failed to add the synonyms type vocabulary");
         return 0;
      }
      $syncv = db_fetch_object(db_query($sql));
   }

   // get the 'exact' cvterm, which is the type of synonym we're adding
   $cvtsql = "
      SELECT * FROM {cvterm} CVT
         INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
      WHERE CVT.name = '%s' and CV.name = '%s'
   ";
   $syntype = db_fetch_object(db_query($cvtsql,'exact','synonym_type'));
   if(!$syntype){
      $term = array(
         'name' => array('exact'),
         'id' => array("internal:exact"),
         'definition' => array(''),
         'is_obsolete' => array(0),
      );
      $syntype = tripal_cv_add_cvterm($term,$syncv,0,1);
      if(!$syntype){
         print("Cannot add synonym type: internal:$type");
         return 0;
      }
   }

   // iterate through all of the aliases and add each one
   foreach($aliases as $alias){
      print "   Adding Alias $alias\n";

      // check to see if the alias already exists in the synonym table
      // if not, then add it
      $synsql = "SELECT * FROM {synonym}
                 WHERE name = '%s' and type_id = %d";
      $synonym = db_fetch_object(db_query($synsql,$alias,$syntype->cvterm_id));
      if(!$synonym){      
         $sql = "INSERT INTO {synonym}
                  (name,type_id,synonym_sgml)
                 VALUES ('%s',%d,'%s')";
         $result = db_query($sql,$alias,$syntype->cvterm_id,'');
         if(!$result){
            print "ERROR: cannot add alias $alias to synonym table\n";
         }
      }           
      $synonym = db_fetch_object(db_query($synsql,$alias,$syntype->cvterm_id));


      // check to see if we have a NULL publication in the pub table.  If not,
      // then add one.
      $pubsql = "SELECT * FROM {pub} WHERE uniquename = 'null'";
      $pub = db_fetch_object(db_query($pubsql));
      if(!$pub){
         $sql = "INSERT INTO pub (uniquename,type_id) VALUES ('%s',
                   (SELECT cvterm_id 
                    FROM cvterm CVT
                      INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
                      INNER JOIN db DB on DB.db_id = DBX.db_id
                    WHERE CVT.name = 'null' and DB.name = 'null')";
         $result = db_query($sql,'null');
         if(!$result){
            print "ERROR: cannot add null publication needed for setup of alias\n";
            return 0;
         }
      }
      $pub = db_fetch_object(db_query($pubsql));

      // check to see if the synonym exists in the feature_synonym table
      // if not, then add it.
      $synsql = "SELECT * FROM {feature_synonym}
                 WHERE synonym_id = %d and feature_id = %d and pub_id = %d";
      $fsyn = db_fetch_object(db_query($synsql,$synonym->synonym_id,$feature->feature_id,$pub->pub_id));
      if(!$fsyn){      
         $sql = "INSERT INTO {feature_synonym}
                  (synonym_id,feature_id,pub_id)
                 VALUES (%d,%d,%d)";
         $result = db_query($sql,$synonym->synonym_id,$feature->feature_id,$pub->pub_id);
         if(!$result){
            print "ERROR: cannot add alias $alias to feature synonym table\n";
            return 0;
         }
      } else {
         print "   Synonym $alias already exists. Skipping\n";
      }
   }
   return 1;
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_feature($organism,$analysis_id,$cvterm,$uniquename,$name,
   $residues,$is_analysis='f',$is_obsolete='f',$add_only,$score)  {

   // check to see if the feature already exists
   $feature_sql = "SELECT * FROM {feature} 
                   WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
   $feature = db_fetch_object(db_query($feature_sql,$organism->organism_id,$uniquename,$cvterm->cvterm_id));

   if(strcmp($is_obsolete,'f')==0){
      $is_obsolete = 'false';
   }
   if(strcmp($is_analysis,'f')==0){
      $is_analysis = 'false';
   }


   // insert the feature if it does not exist otherwise perform an update
   if(!$feature){
      print "Adding feature '$uniquename' ($cvterm->name)\n";
      $isql = "INSERT INTO {feature} (organism_id, name, uniquename, residues, seqlen,
                  md5checksum, type_id,is_analysis,is_obsolete)
               VALUES(%d,'%s','%s','%s',%d, '%s', %d, %s, %s)";
      $result = db_query($isql,$organism->organism_id,$name,$uniquename,$residues,strlen($residues),
               md5($residues),$cvterm->cvterm_id,$is_analysis,$is_obsolete);
      if(!$result){
         print "ERROR: failed to insert feature '$uniquename' ($cvterm->name)\n";
         return 0;
      }
   } 
   elseif(!$add_only) {
      print "Updating feature '$uniquename' ($cvterm->name)\n";
      $usql = "UPDATE {feature} 
               SET name = '%s', residues = '%s', seqlen = '%s', md5checksum = '%s',
                  is_analysis = %s, is_obsolete = %s
               WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
      $result = db_query($usql,$name,$residues,strlen($residues),md5($residues),$is_analysis,$is_obsolete,
                   $organism_id,$uniquename,$cvterm->cvterm_id);
      if(!$result){
         print "ERROR: failed to update feature '$uniquename' ($cvterm->name)\n";
         return 0;
      }
   }
   else {
      // the feature exists and we don't want to update it so return
      // a value of 0.  This will stop all downstream property additions
      print "Skipping existing feature: '$uniquename' ($cvterm->name).\n";
      return 0;
   }

   // get the newly added feature
   $feature = db_fetch_object(db_query($feature_sql,$organism->organism_id,$uniquename,$cvterm->cvterm_id));

   // add the analysisfeature entry to the analysisfeature table if it doesn't already exist
   $af_values = array(
      'analysis_id' => $analysis_id, 
      'feature_id' => $feature->feature_id
   );
   $afeature = tripal_core_chado_select('analysisfeature',array('analysisfeature_id'),$af_values,array('has_record'));
   if(count($afeature)==0){
      // if a score is avaialble then set that to be the significance field
      if(strcmp($score,'.')!=0){
        $af_values['significance'] = $score;
      }
      if(!tripal_core_chado_insert('analysisfeature',$af_values)){
         print "ERROR: could not add analysisfeature record: $analysis_id, $feature->feature_id\n";
      } else {
         print "   Added analysisfeature record\n";
      }
   } else {
      // if a score is available then set that to be the significance field
      $new_vals = array();
      if(strcmp($score,'.')!=0){
        $new_vals['significance'] = $score;
      } else {
        $new_vals['significance'] = '__NULL__';
      }
      if(!$add_only){
         $ret = tripal_core_chado_update('analysisfeature',$af_values,$new_vals);
         if(!$ret){
            print "ERROR: could not update analysisfeature record: $analysis_id, $feature->feature_id\n";
         } else {
            print "   Updated analysisfeature record\n";
         } 
      }
   }

   return $feature;
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_featureloc($feature,$organism,$landmark,$fmin,
   $fmax,$strand,$phase,$is_fmin_partial,$is_fmax_partial,$residue_info,$locgroup)
{
 
   // get the source feature
   $sql = "SELECT * FROM {feature} 
           WHERE organism_id = %d and uniquename = '%s'";
   $srcfeature = db_fetch_object(db_query($sql,$organism->organism_id,$landmark));
   if(!$srcfeature){
      print "ERROR: cannot find landmark feature $landmark.  Cannot add the feature location record\n";
      return 0;
   }


   // TODO: create an attribute that recognizes the residue_info,locgroup, is_fmin_partial and is_fmax_partial, right now these are
   //       hardcoded to be false and 0 below.


   // check to see if this featureloc already exists, but also keep track of the
   // last rank value
   $rank = 0;  
   $exists = 0;  
   $featureloc_sql = "SELECT FL.featureloc_id,FL.fmin,FL.fmax,F.uniquename as srcname,
                         rank
                      FROM {featureloc} FL
                        INNER JOIN {feature} F on F.feature_id = FL.srcfeature_id
                      WHERE FL.feature_id = %d
                      ORDER BY rank ASC";
   $recs = db_query($featureloc_sql,$feature->feature_id);
   while ($featureloc = db_fetch_object($recs)){
      if(strcmp($featureloc->srcname,$landmark)==0 and
         $featureloc->fmin == $fmin and $featureloc->fmax == $fmax){
         // this is the same featureloc, so do nothing... no need to update
         //TODO: need more checks here
         print "   No change to featureloc\n";
         $exists = 1;
      }
      $rank = $featureloc->rank + 1;
   }
   if(!$exists){
      $rank++;
      // this feature location is new so add it
      if(!$phase){
          $phase = 'NULL';
      }
      if(strcmp($is_fmin_partial,'f')==0){
         $is_fmin_partial = 'false';
      }
      elseif(strcmp($is_fmin_partial,'t')==0){
         $is_fmin_partial = 'true';
      }
      if(strcmp($is_fmax_partial,'f')==0){
         $is_fmax_partial = 'false';
      }
      elseif(strcmp($is_fmax_partial,'t')==0){
         $is_fmax_partial = 'true';
      }
      print "   Adding featureloc $srcfeature->uniquename fmin: $fmin, fmax: $fmax, strand: $strand, phase: $phase, rank: $rank\n";
      $fl_isql = "INSERT INTO {featureloc} 
                    (feature_id, srcfeature_id, fmin, is_fmin_partial, fmax, is_fmax_partial,
                     strand, phase, residue_info, locgroup, rank) 
                 VALUES (%d,%d,%d,%s,%d,%s,%d,%s,'%s',%d,%d)";
      $result = db_query($fl_isql,$feature->feature_id,$srcfeature->feature_id,$fmin,$is_fmin_partial,$fmax,$is_fmax_partial,
               $strand,$phase,$residue_info,$locgroup,$rank);
      if(!$result){
         print "ERROR: failed to insert featureloc\n";
         exit;
         return 0;
      }
   }
   return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_property($feature,$property,$value){
   // first make sure the cvterm exists.  If the term already exists then
   // the function should return it of not, then add it
   $cvt_sql = "SELECT * FROM {cvterm} CVT
               INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
               WHERE CV.name = '%s' and CVT.name = '%s'";
   $cvterm = db_fetch_object(db_query($cvt_sql,'feature_property',$property));
   if(!$cvterm){
      $term = array(
         'id' => "null:$property",
         'name' => $property,
         'namespace' => 'feature_property', 
         'is_obsolete' => 0,
      );
      print "   Adding cvterm, $property\n";
      $cvterm = (object) tripal_cv_add_cvterm($term,'feature_property',0,0);
   }

   if(!$cvterm){
      print "ERROR: cannot add cvterm, $property\n";
      exit;
   }

   // check to see if the property already exists for this feature
   // if it does but the value is unique then increment the rank and add it. 
   // if the value is not unique then don't add it.
   $add = 1;
   $rank = 0;
   $sql = "SELECT rank,value FROM {featureprop} 
           WHERE feature_id = %d and type_id = %d
           ORDER BY rank ASC";
   $result = db_query($sql,$feature->feature_id,$cvterm->cvterm_id);
   while($prop = db_fetch_object($result)){
      if(strcmp($prop->value,$value)==0){
        $add = NULL; // don't add it, it already exists
        print "   Property already exists, skipping\n";
      }
      $rank = $prop->rank + 1;
   }
   
   // add the property if we pass the check above
   if($add){
      print "   Setting feature property. $property: $value\n";
      $isql = "INSERT INTO {featureprop} (feature_id,type_id,value,rank)
               VALUES (%d,%d,'%s',%d)";
      db_query($isql,$feature->feature_id,$cvterm->cvterm_id,$value,$rank);
   }
}
/*
function tripal_feature_load_gff3_property($feature,$property,$value){
   // first make sure the cvterm exists.  If the term already exists then
   // the function should return it
   $match = array(
      'name' => $property,
      'cv_id' => array(
         'name' => 'feature_property',
      ),
   );
   $cvterm = tripal_core_chado_select('cvterm',array('*'),$match);
   if(sizeof($cvterm) == 0){
      $term = array(
         'id' => "null:$property",
         'name' => $property,
         'namespace' => 'feature_property', 
         'is_obsolete' => 0,
      );
      print "   Adding cvterm, $property\n";
      $cvterm = tripal_cv_add_cvterm($term,'feature_property',0,0);
   }

   if(!$cvterm){
      print "ERROR: cannot add cvterm, $property, before adding property\n";
      exit;
   }

   // next give the feature the property
   tripal_core_insert_property('feature',$feature->feature_id,$property,'feature_property',$value,1);
}
*/

