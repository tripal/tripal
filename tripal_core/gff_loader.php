<?php

/*************************************************************************
*
*/
function tripal_core_gff3_load_form (){

   $form['gff_file']= array(
      '#type'          => 'textfield',
      '#title'         => t('GFF3 File'),
      '#description'   => t('Please enter the full system path for the GFF file, or a path within the Drupal
                             installation (e.g. /sites/default/files/xyz.obo).  The path must be accessible to the
                             server on which this Drupal instance is running.'),
      '#required' => TRUE,
      '#weight'        => 1
   );
   $form['add_only']= array(
      '#type' => 'checkbox',
      '#title' => t('Import only new features'),
      '#required' => FALSE,
      '#default_value' => 'checked',
      '#description' => t('The job will skip features in the GFF file that already
                           exist in the database and import only new features.'),
      '#weight' => 2
   );
   $form['update']= array(
      '#type' => 'checkbox',
      '#title' => t('Import all and update'),
      '#required' => FALSE,
      '#description' => t('Existing features will be updated and new features will be added.  Attributes 
                           for a feature that are not present in the GFF but which are present in the 
                           database will not be altered.'),
      '#weight' => 3
   );
   $form['refresh']= array(
      '#type' => 'checkbox',
      '#title' => t('Import all and replace'),
      '#required' => FALSE,
      '#description' => t('Existing features will be updated and feature properties not
                           present in the GFF file will be removed.'),
      '#weight' => 4
   );
   $form['remove']= array(
      '#type' => 'checkbox',
      '#title' => t('Delete features'),
      '#required' => FALSE,
      '#description' => t('Features present in the GFF file that exist in the database
                           will be removed rather than imported'),
      '#weight' => 5
   );
   $form['button'] = array(
      '#type' => 'submit',
      '#value' => t('Import GFF3 file'),
      '#weight' => 10,
   );


   return $form;
}
/*************************************************************************
*
*/
function tripal_core_gff3_load_form_submit ($form, &$form_state){
   global $user;

   $gff_file = $form_state['values']['gff_file'];
   $add_only = $form_state['values']['add_only'];
   $update   = $form_state['values']['update'];
   $refresh  = $form_state['values']['refresh'];
   $remove   = $form_state['values']['remove'];

   $args = array($gff_file,$add_only,$update,$refresh,$remove);
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
   tripal_add_job("Import GFF3 file: $type",'tripal_core',
      'tripal_core_load_gff3',$args,$user->uid);

   return '';
}
/*************************************************************************
*
*/
function tripal_core_load_gff3($gff_file, $add_only =0, $update = 0, $refresh = 0, $remove = 0, $job = NULL){
   
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
    
   $lines = file($dfile,FILE_SKIP_EMPTY_LINES);
   $i = 0;

   // get the controlled vocaubulary that we'll be using.  The
   // default is the 'sequence' ontology
   $vocab = 'sequence';
   $sql = "SELECT * FROM cv WHERE name = '%s'";
   $cv = db_fetch_object(db_query($sql,$vocab));
   if(!$cv){
      print "ERROR:  cannot find the '$vocab' ontology\n";
      return '';
   }

   // get the organism for which this GFF3 file belongs
   $sql = "SELECT * FROM organism WHERE common_name = 'fruitfly'";
   $organism = db_fetch_object(db_query($sql));

   foreach ($lines as $line_num => $line) {
      $i++;  // update the line count

      if(preg_match('/^#/',$line)){
         continue; // skip comments
      }
      

      // TODO: remove URL encoding
  
      $cols = explode("\t",$line);
      if(sizeof($cols) != 9){
         print "ERROR: improper number of columns on line $i\n";
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

      // ready the start and stop for chado
      $fmin = $start;
      $fmax = $end;
      if($end < $start){
         $fmin = $end;
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
      $sql = "SELECT * from cvterm CVT WHERE name = '%s' and cv_id = %d";
      $cvterm = db_fetch_object(db_query($sql,$type,$cv->cv_id));
      if(!$cvterm){
         print "ERROR: cannot find ontology term '$type' on line $i.\n";
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
      $residues = '';
      foreach($attrs as $attr){
         $attr = rtrim($attr);
         $attr = ltrim($attr);
         if(strcmp($attr,'')==0){
            continue;
         }
         if(!preg_match('/^[^\=]+\=[^\=]+$/',$attr)){
            print "ERROR: attribute is not correctly formatted on line $i: $attr\n";
            return '';
         }

         // break apart each tag
         $tag = explode("=",$attr);  // split by equals sign
         // multiple instances of an attribute are separated by commas
         $tags[$tag[0]] = explode(",",$tag[1]);  // split by comma
         if(strcmp($tag[0],'ID')==0){
            $attr_uniquename = $tag[1];
         }
         if(strcmp($tag[0],'Name')==0){
            $attr_name = $tag[1];
         }
      }
      if(strcmp($attr_name,'')==0){
         $attr_name = $attr_uniquename;
      }

      // skip features that have no ID attribute
      if(!$attr_uniquename){
         continue;
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
         $feature = tripal_core_load_gff3_feature($organism,$cvterm,
            $attr_uniquename,$attr_name,$residues,$attr_is_analysis,$attr_is_obsolete,
            $add_only);
         if($feature){
            // add/update the featureloc if the landmark and the feature ID are not the same
            // if they are the same then this entry in the GFF is probably a landmark identifier
            if(strcmp($landmark,$attr_uniquename)!=0){
               tripal_core_load_gff3_featureloc($feature,$organism,
                  $landmark,$fmin,$fmax,$strand,$phase,$attr_fmin_partial,
                  $attr_fmax_partial,$attr_residue_info,$attr_locgroup);
            }
            // add any aliases for this feature
            if(array_key_exists('Alias',$tags)){
               tripal_core_load_gff3_alias($feature,$tags['Alias']);
            }
            // add any aliases for this feature
            if(array_key_exists('Dbxref',$tags)){
               tripal_core_load_gff3_dbxref($feature,$tags['Dbxref']);
            }
         }
      }
   }
   tripal_db_set_active($previous_db);
   return '';
}
/*************************************************************************
*
*/
function tripal_core_load_gff3_dbxref($feature,$dbxrefs){
   foreach($dbxrefs as $dbxref){
      print "Adding: $dbxref\n";
   }
}

/*************************************************************************
*
*/
function tripal_core_load_gff3_alias($feature,$aliases){

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
      $syntype = tripal_cv_obo_add_cv_term($term,$syncv,0,1);
      if(!$syntype){
         tripal_cv_obo_quiterror("Cannot add synonym type: internal:$type");
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
      }
      $fsyn = db_fetch_object(db_query($synsql,$synonym->synonym_id,$feature->feature_id,$pub->pub_id));
   }
   return 1;
}
/*************************************************************************
*
*/
function tripal_core_load_gff3_feature($organism,$cvterm,$uniquename,$name,
   $residues,$is_analysis='f',$is_obsolete='f',$add_only)  {

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
      print "Adding feature '$uniquename'\n";
      $sql = "INSERT INTO {feature} (organism_id, name, uniquename, residues, seqlen,
                 md5checksum, type_id,is_analysis,is_obsolete)
              VALUES(%d,'%s','%s','%s',%d, '%s', %d, %s, %s)";
      $result = db_query($sql,$organism->organism_id,$name,$uniquename,$residues,strlen($residues),
               md5($residues),$cvterm->cvterm_id,$is_analysis,$is_obsolete);
      if(!$result){
         print "ERROR: failed to insert feature '$uniquename'\n";
         return 0;
      }
   } 
   elseif(!$add_only) {
      print "Updating feature '$uniquename'\n";
      $sql = "UPDATE {feature} 
              SET name = '%s', residues = '%s', seqlen = '%s', md5checksum = '%s',
                 is_analysis = %s, is_obsolete = %s
              WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
      $result = db_query($sql,$name,$residues,strlen($residues),md5($residues),$is_analysis,$is_obsolete);
      if(!$result){
         print "ERROR: failed to update feature '$uniquename'\n";
         return 0;
      }
   }
   else {
      // the feature exists and we don't want to update it so return
      // a value of 0.  This will stop all downstream property additions
      print "Skipping existing feature: '$uniquename'.\n";
      return 0;
   }

   $feature = db_fetch_object(db_query($feature_sql,$organism->organism_id,$uniquename,$cvterm->cvterm_id));
   return $feature;
}
/*************************************************************************
*
*/
function tripal_core_load_gff3_featureloc($feature,$organism,$landmark,$fmin,$fmax,$strand,$phase,
   $is_fmin_partial,$is_fmax_partial,$residue_info,$locgroup)  {
 
   // get the source feature
   $sql = "SELECT * FROM {feature} 
           WHERE organism_id = %d and uniquename = '%s'";
   $srcfeature = db_fetch_object(db_query($sql,$organism->organism_id,$landmark));
   if(!$srcfeature){
      print "ERROR: cannot find source feature $landmark.\n";
      return 0;
   }


   // TODO: create an attribute that recognizes the residue_info,locgroup, is_fmin_partial and is_fmax_partial, right now these are
   //       hardcoded to be false and 0 below.


   // check to see if this featureloc already exists, but also keep track of the
   // last rank value
   $rank = 0;  
   $exists = 0;  
   $featureloc_sql = "SELECT FL.featureloc_id,FL.fmin,FL.fmax, FL.is_fmin_partial,
                         FL.is_fmax_partial, FL.strand, FL.phase, FL.residue_info,
                         FL.locgroup, F.uniquename as srcname
                      FROM {featureloc} FL
                        INNER JOIN {feature} F on F.feature_id = FL.srcfeature_id
                      WHERE FL.feature_id = %d
                      ORDER BY rank ASC";
   $recs = db_query($featureloc_sql,$feature->feature_id);
   while ($featureloc = db_fetch_object($recs)){
      if(strcmp($featureloc->srcname,$landmark)==0 and
         $featureloc->fmin == $fmin and strcmp($featureloc->is_fmin_partial,$is_fmin_partial)==0 and
         $featureloc->fmax == $fmax and strcmp($featureloc->is_fmax_partial,$is_fmax_partial)==0 and
         $featureloc->phase == $phase and $featureloc->strand == $strand and
         strcmp($featureloc->residue_info,$residue_info)==0 and 
         $featureloc->locgroup == $locgroup){
         // this is the same featureloc, so do nothing... no need to update
         //TODO: need more checks here
         print "   No change to featureloc\n";
         $exists = 1;
      }
      $rank = $featureloc->rank;
   }
   if(!$exists){
      // this feature does not have a feature loc so add it
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
         return 0;
      }
   }
   return 1;
}
/*************************************************************************
*
*/

