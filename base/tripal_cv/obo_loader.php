<?php

/** 
 * @defgroup tripal_obo_loader Tripal Ontology Loader
 * @ingroup tripal_cv
 */
 
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_load_obo_v1_2_id($obo_id,$jobid = NULL){

   // get the OBO reference
   $sql = "SELECT * FROM {tripal_cv_obo} WHERE obo_id = %d";
   $obo = db_fetch_object(db_query($sql,$obo_id));

   // if the reference is for a remote URL then run the URL processing function
   if(preg_match("/^http:\/\//",$obo->path) or preg_match("/^ftp:\/\//",$obo->path)){
      tripal_cv_load_obo_v1_2_url($obo->name,$obo->path,$jobid,0);
   } 
   // if the reference is for a local file then run the file processing function
   else {
      // check to see if the file is located local to Drupal
      $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $obo->path; 
      if(file_exists($dfile)){
         tripal_cv_load_obo_v1_2_file($obo->name,$dfile,$jobid,0);
      } 
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      else{
         if(file_exists($obo->path)){
            tripal_cv_load_obo_v1_2_file($obo->name,$obo->path,$jobid,0);
         } else {
            print "ERROR: counld not find OBO file: '$obo->path'\n";
         }
      }
   }  
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_load_obo_v1_2_file($obo_name,$file,$jobid = NULL,$is_new = 1){
   $newcvs = array();

   tripal_cv_load_obo_v1_2($file,$jobid,$newcvs);
   if($is_new){
      tripal_cv_load_obo_add_ref($obo_name,$file);
   }
   // update the cvtermpath table 
   tripal_cv_load_update_cvtermpath($newcvs,$jobid);
   print "Ontology Sucessfully loaded!\n";  
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_load_obo_v1_2_url($obo_name,$url,$jobid = NULL,$is_new = 1){

   $newcvs = array();

   // first download the OBO
   $temp = tempnam(sys_get_temp_dir(),'obo_');
   print "Downloading URL $url, saving to $temp\n";
   $url_fh = fopen($url,"r");
   $obo_fh = fopen($temp,"w");
   while(!feof($url_fh)){
      fwrite($obo_fh,fread($url_fh,255),255);
   }
   fclose($url_fh);
   fclose($obo_fh);

   // second, parse the OBO
   tripal_cv_load_obo_v1_2($temp,$jobid,$newcvs);

   // now remove the temp file
   unlink($temp);

   if($is_new){
      tripal_cv_load_obo_add_ref($obo_name,$url);
   }

   // update the cvtermpath table 
   tripal_cv_load_update_cvtermpath($newcvs,$jobid);

   print "Ontology Sucessfully loaded!\n";
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_load_update_cvtermpath($newcvs,$jobid){

   print "\nUpdating cvtermpath table.  This may take a while...\n";
   foreach($newcvs as $namespace => $cvid){
      tripal_cv_update_cvtermpath($cvid, $jobid);
   }
}
/**
*
*/
function tripal_cv_load_obo_add_ref($name,$path){
   $isql = "INSERT INTO tripal_cv_obo (name,path) VALUES ('%s','%s')";
   db_query($isql,$name,$path);
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_load_obo_v1_2($file,$jobid = NULL,&$newcvs) {

   global $previous_db;

   $header = array();
   $obo = array();
   
   print "Opening File $file\n";

   // set the search path
   $previous_db = tripal_db_set_active('chado');

   // make sure we have an 'internal' and a '_global' database
   if(!tripal_cv_obo_add_db('internal')){
      tripal_cv_obo_quiterror("Cannot add 'internal' database");
   }
   if(!tripal_cv_obo_add_db('_global')){
      tripal_cv_obo_quiterror("Cannot add '_global' database");
   }

   // parse the obo file
   tripal_cv_obo_parse($file,$obo,$header);

   // add the CV for this ontology to the database
   $defaultcv = tripal_cv_obo_add_cv($header['default-namespace'][0],'');
   if(!$defaultcv){
      tripal_cv_obo_quiterror('Cannot add namespace ' . $header['default-namespace'][0]);
   }  
   $newcvs[$header['default-namespace'][0]] = $defaultcv->cv_id;

   // add any typedefs to the vocabulary first
   $typedefs = $obo['Typedef'];
   foreach($typedefs as $typedef){
      tripal_cv_obo_process_term($typedef,$defaultcv,$obo,1,$newcvs);  
   }

   // next add terms to the vocabulary
   $terms = $obo['Term'];
   if(!tripal_cv_obo_process_terms($terms,$defaultcv,$obo,$jobid,$newcvs)){
      tripal_cv_obo_quiterror('Cannot add terms from this ontology');
   }
   return tripal_cv_obo_loader_done();
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_quiterror ($message){
   print "ERROR: $message\n";
   db_query("set search_path to public");  
   exit;
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_loader_done (){
   // return the search path to normal
   tripal_db_set_active($previous_db);
   db_query("set search_path to public");  
   return '';
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_process_terms($terms,$defaultcv,$obo,$jobid=null,&$newcvs){

   $i = 0;
   $count = sizeof($terms);
   $interval = intval($count * 0.01);
   if($interval > 1){
      $interval = 1;
   }

   foreach ($terms as $term){

      // update the job status every 1% terms
      if($jobid and $i % $interval == 0){
         tripal_job_set_progress($jobid,intval(($i/$count)*50)); // we mulitply by 50 because parsing and loacing cvterms
                                                                 // is only the first half.  The other half is updating
      }                                                          // the cvtermpath table.

      if(!tripal_cv_obo_process_term($term,$defaultcv,$obo,0,$newcvs)){
         tripal_cv_obo_quiterror("Failed to process terms from the ontology");
      }
      $i++;
   }
   return 1;
}

/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_process_term($term,$defaultcv,$obo,$is_relationship=0,&$newcvs){

   // add the cvterm
   $cvterm = tripal_cv_obo_add_cvterm($term,$defaultcv,$is_relationship,1);     
   if(!$cvterm){ 
      tripal_cv_obo_quiterror("Cannot add the term " . $term['id'][0]);
   }
   if($term['namespace'][0]){
      $newcvs[$term['namespace'][0]] = $cvterm->cv_id;
   }

   // now handle other properites
   if(isset($term['is_anonymous'])){
     print "WARNING: unhandled tag: is_anonymous\n";
   }
   if(isset($term['alt_id'])){
      foreach($term['alt_id'] as $alt_id){
         if(!tripal_cv_obo_add_cvterm_dbxref($cvterm,$alt_id)){
            tripal_cv_obo_quiterror("Cannot add alternate id $alt_id");
         }
      }
   }
   if(isset($term['subset'])){
     print "WARNING: unhandled tag: subset\n";
   }
   // add synonyms for this cvterm
   if(isset($term['synonym'])){
      if(!tripal_cv_obo_add_synonyms($term,$cvterm)){
         tripal_cv_obo_quiterror("Cannot add synonyms");
      }
   }
   // reformat the deprecated 'exact_synonym, narrow_synonym, and broad_synonym'
   // types to be of the v1.2 standard
   if(isset($term['exact_synonym']) or isset($term['narrow_synonym']) or isset($term['broad_synonym'])){
      if(isset($term['exact_synonym'])){
         foreach ($term['exact_synonym'] as $synonym){

            $new = preg_replace('/^\s*(\".+?\")(.*?)$/','$1 EXACT $2',$synonym);
            $term['synonym'][] = $new;
         }
      }
      if(isset($term['narrow_synonym'])){
         foreach ($term['narrow_synonym'] as $synonym){
            $new = preg_replace('/^\s*(\".+?\")(.*?)$/','$1 NARROW $2',$synonym);
            $term['synonym'][] = $new;
         }
      }
      if(isset($term['broad_synonym'])){
         foreach ($term['broad_synonym'] as $synonym){
            $new = preg_replace('/^\s*(\".+?\")(.*?)$/','$1 BROAD $2',$synonym);
            $term['synonym'][] = $new;
         }
      } 

      if(!tripal_cv_obo_add_synonyms($term,$cvterm)){
         tripal_cv_obo_quiterror("Cannot add/update synonyms");
      }
   }
   // add the comment to the cvtermprop table
   if(isset($term['comment'])){
      $comments = $term['comment'];
      $j = 0;
      foreach($comments as $comment){
         if(!tripal_cv_obo_add_cvterm_prop($cvterm,'comment',$comment,$j)){
            tripal_cv_obo_quiterror("Cannot add/update cvterm property");
         }
         $j++;
      }
   }    
   // add any other external dbxrefs
   if(isset($term['xref'])){
      foreach($term['xref'] as $xref){
         if(!tripal_cv_obo_add_cvterm_dbxref($cvterm,$xref)){
            tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
         }
      }
   }
   if(isset($term['xref_analog'])){
      foreach($term['xref_analog'] as $xref){
         if(!tripal_cv_obo_add_cvterm_dbxref($cvterm,$xref)){
            tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
         }
      }
   }
   if(isset($term['xref_unk'])){
      foreach($term['xref_unk'] as $xref){
         if(!tripal_cv_obo_add_cvterm_dbxref($cvterm,$xref)){
            tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
         }
      }
   }

   // add is_a relationships for this cvterm
   if(isset($term['is_a'])){
      foreach($term['is_a'] as $is_a){
         if(!tripal_cv_obo_add_relationship($cvterm,$defaultcv,$obo,'is_a',$is_a,$is_relationship)){
            tripal_cv_obo_quiterror("Cannot add relationship is_a: $is_a");
         }
      }
   } 
   if(isset($term['intersection_of'])){
     print "WARNING: unhandled tag: intersection_of\n";
   }
   if(isset($term['union_of'])){
     print "WARNING: unhandled tag: union_on\n";
   }
   if(isset($term['disjoint_from'])){
     print "WARNING: unhandled tag: disjoint_from\n";
   }
   if(isset($term['relationship'])){
      foreach($term['relationship'] as $value){
         $rel = preg_replace('/^(.+?)\s.+?$/','\1',$value);
         $object = preg_replace('/^.+?\s(.+?)$/','\1',$value);
         if(!tripal_cv_obo_add_relationship($cvterm,$defaultcv,$obo,$rel,$object,$is_relationship)){
            tripal_cv_obo_quiterror("Cannot add relationship $rel: $object");
         }
      }
   }
   if(isset($term['replaced_by'])){
     print "WARNING: unhandled tag: replaced_by\n";
   }
   if(isset($term['consider'])){
     print "WARNING: unhandled tag: consider\n";
   }
   if(isset($term['use_term'])){
     print "WARNING: unhandled tag: user_term\n";
   }
   if(isset($term['builtin'])){
     print "WARNING: unhandled tag: builtin\n";
   }
   return 1;
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_add_db($dbname){

   $db_sql = "SELECT * FROM {db} WHERE name ='%s'";
   $db = db_fetch_object(db_query($db_sql,$dbname));
   if(!$db){
      if(!db_query("INSERT INTO {db} (name) VALUES ('%s')",$dbname)){
         tripal_cv_obo_quiterror("Cannot create '$dbname' db in Chado.");
      }      
     $db = db_fetch_object(db_query($db_sql,$dbname));
   }
   return $db;
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_add_cv($name,$comment){

 // see if the CV (default-namespace) exists already in the database
   $vocab = $name;
   $remark = $comment;
   $cv_sql = "SELECT * FROM {cv} WHERE name = '%s'";
   $cv = db_fetch_object(db_query($cv_sql,$vocab));

   // if the CV exists then update it, otherwise insert
   if(!$cv){
      $sql = "INSERT INTO {cv} (name,definition) VALUES ('%s','%s')";
      if(!db_query($sql,$vocab,$remark)){
         tripal_cv_obo_quiterror("Failed to create the CV record");
      }
      $cv = db_fetch_object(db_query($cv_sql,$vocab));
   } else {
      $sql = "UPDATE {cv} SET definition = '%s' WHERE name ='%s'";
      if(!db_query($sql,$remark,$vocab)){
         tripal_cv_obo_quiterror("Failed to update the CV record");
      }
      $cv = db_fetch_object(db_query($cv_sql,$vocab));
   }
   return $cv;
}

/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_add_relationship($cvterm,$defaultcv,$obo,$rel,$objname,$object_is_relationship=0){

   // make sure the relationship cvterm exists
   $term = array(
      'name' => array($rel),
      'id' => array($rel),
      'definition' => array(''),
      'is_obsolete' => array(0),
   );
   $relcvterm = tripal_cv_obo_add_cvterm($term,$defaultcv,1,0);
   if(!$relcvterm){
      tripal_cv_obo_quiterror("Cannot find or insert the relationship term: $rel\n");
   }

   // get the object term
   $objterm = tripal_cv_obo_get_term($obo,$objname);
   if(!$objterm) { 
      tripal_cv_obo_quiterror("Could not find object term $objname\n"); 
   }
   $objcvterm = tripal_cv_obo_add_cvterm($objterm,$defaultcv,$object_is_relationship,1);
   if(!$objcvterm){ 
      tripal_cv_obo_quiterror("Cannot add/find cvterm");
   }

   // check to see if the cvterm_relationship already exists, if not add it
   $cvrsql = "SELECT * FROM {cvterm_relationship} WHERE type_id = %d and subject_id = %d and object_id = %d";
   if(!db_fetch_object(db_query($cvrsql,$relcvterm->cvterm_id,$cvterm->cvterm_id,$objcvterm->cvterm_id))){
      $sql = "INSERT INTO {cvterm_relationship} ".
             "(type_id,subject_id,object_id) VALUES (%d,%d,%d)";
      if(!db_query($sql,$relcvterm->cvterm_id,$cvterm->cvterm_id,$objcvterm->cvterm_id)){
         tripal_cv_obo_quiterror("Cannot add term relationship: '$cvterm->name' $rel '$objcvterm->name'");
      }
   }

   return 1;
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_get_term($obo,$id){
   foreach ($obo as $type){
      foreach ($type as $term){
         $accession = $term['id'][0];
         if(strcmp($accession,$id)==0){
            return $term;
         }
      }
   }
   return;
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_add_synonyms($term,$cvterm){

   // make sure we have a 'synonym_type' vocabulary
   $sql = "SELECT * FROM {cv} WHERE name='synonym_type'";
   $syncv = db_fetch_object(db_query($sql));
   if(!$syncv){
      $sql = "INSERT INTO {cv} (name,definition) VALUES ('synonym_type','')";
      if(!db_query($sql)){
         tripal_cv_obo_quiterror("Failed to add the synonyms type vocabulary");
      }
      $syncv = db_fetch_object(db_query($sql));
   }

   // now add the synonyms
   if(isset($term['synonym'])){
      foreach($term['synonym'] as $synonym){
         // separate out the synonym definition and the synonym type
         $def = preg_replace('/^\s*"(.*)"\s*.*$/','\1',$synonym);
         $type = strtolower(preg_replace('/^.*"\s+(.*?)\s+.*$/','\1',$synonym)); 

         // make sure the synonym type exists in the 'synonym_type' vocabulary
         $cvtsql = "
            SELECT * 
            FROM {cvterm} CVT
               INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
            WHERE CVT.name = '%s' and CV.name = '%s'
         ";
         $syntype = db_fetch_object(db_query($cvtsql,$type,'synonym_type'));
         if(!$syntype){
            // build a 'term' object so we can add the missing term
            $term = array(
               'name' => array($type),
               'id' => array("internal:$type"),
               'definition' => array(''),
               'is_obsolete' => array(0),
            );
            $syntype = tripal_cv_obo_add_cvterm($term,$syncv,0,1);
            if(!$syntype){
               tripal_cv_obo_quiterror("Cannot add synonym type: internal:$type");
            }
         }       

         // make sure the synonym doesn't already exists
         $sql = "
            SELECT * 
            FROM {cvtermsynonym} 
            WHERE cvterm_id = %d and synonym = '%s'
         ";
         $syn = db_fetch_object(db_query($sql,$cvterm->cvterm_id,$def));
         if(!$syn){
            $sql = "INSERT INTO {cvtermsynonym} (cvterm_id,synonym,type_id)
                    VALUES(%d,'%s',%d)";
            if(!db_query($sql,$cvterm->cvterm_id,$def,$syntype->cvterm_id)){
               tripal_cv_obo_quiterror("Failed to insert the synonym for term: $name ($def)");
            }
         } 

         // now add the dbxrefs for the synonym if we have a comma in the middle
         // of a description then this will cause problems when splitting os lets
         // just change it so it won't mess up our splitting and then set it back
         // later.
//         $synonym = preg_replace('/(".*?),\s(.*?")/','$1,_$2',$synonym);
//         $dbxrefs = preg_split("/, /",preg_replace('/^.*\[(.*?)\]$/','\1',$synonym));
//         foreach($dbxrefs as $dbxref){
//            $dbxref = preg_replace('/,_/',", ",$dbxref);
//            if($dbxref){
//               tripal_cv_obo_add_cvterm_dbxref($syn,$dbxref);
//            }
//         }
      } 
   }
   return 1;
}

/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_parse($obo_file,&$obo,&$header){
   $i = 0;
   $in_header = 1;
   $stanza = array();

   // iterate through the lines in the OBO file and parse the stanzas
   $fh = fopen($obo_file,'r');
   while($line = fgets($fh)) {
      $i++;

      // remove newlines
      $line = rtrim($line);  

      // remove any special characters that may be hiding
      $line = preg_replace('/[^(\x20-\x7F)]*/','', $line);

      // skip empty lines
      if(strcmp($line,'')==0) { continue; }

      //remove comments from end of lines
      $line = preg_replace('/^(.*?)\!.*$/','\1',$line);  // TODO: if the explamation is escaped

      if(preg_match('/^\s*\[/',$line)){  // at the first stanza we're out of header
         $in_header = 0;
         // load the stanza we just finished reading
         if(sizeof($stanza) > 0){
            if(!isset($obo[$type])){
               $obo[$type] = array();
            }
            if(!isset($obo[$type][$stanza['id'][0]])){
               $obo[$type][$stanza['id'][0]] = $stanza;
            } else {
               array_merge($obo[$type][$stanza['id'][0]],$stanza);
            }
         } 
         // get the stanza type:  Term, Typedef or Instance
         $type = preg_replace('/^\s*\[\s*(.+?)\s*\]\s*$/','\1',$line);

         // start fresh with a new array
         $stanza = array();
         continue;
      }
      // break apart the line into the tag and value but ignore any escaped colons
      preg_replace("/\\:/","|-|-|",$line); // temporarily replace escaped colons
      $pair = explode(":",$line,2);
      $tag = $pair[0];
      $value = ltrim(rtrim($pair[1]));// remove surrounding spaces
      $tag = preg_replace("/\|-\|-\|/","\:",$tag); // return the escaped colon
      $value = preg_replace("/\|-\|-\|/","\:",$value);
      if($in_header){
         if(!isset($header[$tag])){
            $header[$tag] = array();
         }
         $header[$tag][] = $value;
      } else {
         if(!isset($stanza[$tag])){
            $stanza[$tag] = array();
         }  
         $stanza[$tag][] = $value;
      }          
   }
   // now add the last term in the file
   if(sizeof($stanza) > 0){
      if(!isset($obo[$type])){
         $obo[$type] = array();
      }
      if(!isset($obo[$type][$stanza['id'][0]])){
         $obo[$type][$stanza['id'][0]] = $stanza;
      } else {
         array_merge($obo[$type][$stanza['id'][0]],$stanza);
      }
   }
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_add_cvterm_dbxref($cvterm,$xref){

   $dbname = preg_replace('/^(.+?):.*$/','$1',$xref);
   $accession = preg_replace('/^.+?:\s*(.*?)(\{.+$|\[.+$|\s.+$|\".+$|$)/','$1',$xref);
   $description = preg_replace('/^.+?\"(.+?)\".*?$/','$1',$xref);
   $dbxrefs = preg_replace('/^.+?\[(.+?)\].*?$/','$1',$xref);

   if(!$accession){
      tripal_cv_obo_quiterror();
      watchdog('tripal_cv',"Cannot add a dbxref without an accession: '$xref'"
         ,NULL,WATCHDOG_WARNING);
      return 0;
   }

   // if the xref is a database link, handle that specially
   if(strcmp($dbname,'http')==0){
      $accession = $xref;
      $dbname = 'URL';
   }

   // check to see if the database exists
   $db = tripal_cv_obo_add_db($dbname);
   if(!$db){
      tripal_cv_obo_quiterror("Cannot find database '$dbname' in Chado.");
   }

   // now add the dbxref
   $dbxref = tripal_cv_obo_add_dbxref($db->db_id,$accession,'',$description);
   if(!$dbxref){ 
      tripal_cv_obo_quiterror("Cannot find or add the database reference (dbxref)");
   }

   // finally add the cvterm_dbxref but first check to make sure it exists
   $sql = "SELECT * from {cvterm_dbxref} WHERE cvterm_id = %d and dbxref_id = %d";
   if(!db_fetch_object(db_query($sql,$cvterm->cvterm_id,$dbxref->dbxref_id))){            
      $sql = "INSERT INTO {cvterm_dbxref} (cvterm_id,dbxref_id)".
             "VALUES (%d,%d)";
      if(!db_query($sql,$cvterm->cvterm_id,$dbxref->dbxref_id)){
         tripal_cv_obo_quiterror("Cannot add cvterm_dbxref: $xref");
      }
   }
   return 1;
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_add_cvterm_prop($cvterm,$property,$value,$rank){

   // make sure the 'cvterm_property_type' CV exists
   $cv = tripal_cv_obo_add_cv('cvterm_property_type','');
   if(!$cv){ 
      tripal_cv_obo_quiterror("Cannot add/find cvterm_property_type cvterm");
   }

   // get the property type cvterm.  If it doesn't exist then we want to add it
   $sql = "
        SELECT * 
        FROM {cvterm} CVT INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
        WHERE CVT.name = '%s' and CV.name = '%s'
   ";
   $cvproptype = db_fetch_object(db_query($sql,$property,'cvterm_property_type'));
   if(!$cvproptype){
      $term = array(
         'name' => array($property),
         'id' => array("internal:$property"),
         'definition' => array(''),
         'is_obsolete' => array(0),
      );
      $cvproptype = tripal_cv_obo_add_cvterm($term,$cv,0,0);
      if(!$cvproptype){  
         tripal_cv_obo_quiterror("Cannot add cvterm property: internal:$property");
      }
   }


   // remove any properties that currently exist for this term.  We'll reset them
   if($rank == 0){
      $sql = "DELETE FROM {cvtermprop} WHERE cvterm_id = %d";
      db_query($sql,$cvterm->cvterm_id);
   }

   // now add the property
   $sql = "INSERT INTO {cvtermprop} (cvterm_id,type_id,value,rank) ".
          "VALUES (%d, %d, '%s',%d)";
   if(!db_query($sql,$cvterm->cvterm_id,$cvproptype->cvterm_id,$value,$rank)){
      tripal_cv_obo_quiterror("Could not add property $property for term\n");
   }
   return 1;
}
/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_add_cvterm($term,$defaultcv,$is_relationship = 0,$update = 1){

   // get the term properties
   $id = $term['id'][0];
   $name = $term['name'][0];
   $cvname = $term['namespace'][0];
   $definition = preg_replace('/^\"(.*)\"/','\1',$term['def'][0]);
   $is_obsolete = 0;
   if(isset($term['is_obsolete'][0]) and  strcmp($term['is_obsolete'][0],'true')==0){
     $is_obsolete = 1;
   }
   if(!$cvname){
      $cvname = $defaultcv->name;
   }
   // make sure the CV name exists
   $cv = tripal_cv_obo_add_cv($cvname,'');
   if(!$cv){
      tripal_cv_obo_quiterror("Cannot find namespace '$cvname' when adding/updating $id");
   }

   // this SQL statement will be used a lot to find a cvterm so just set it
   // here for easy reference below.
   $cvtermsql = "SELECT CVT.name, CVT.cvterm_id, DB.name as dbname, DB.db_id 
                  FROM {cvterm} CVT
                    INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
                    INNER JOIN {db} DB on DBX.db_id = DB.db_id
                    INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
                  WHERE CVT.name = '%s' and DB.name = '%s'";  

   // get the accession and the database from the cvterm
   if(preg_match('/^.+?:.*$/',$id)){
      $accession = preg_replace('/^.+?:(.*)$/','\1',$id);
      $dbname = preg_replace('/^(.+?):.*$/','\1',$id);
   } 
   if($is_relationship and !$dbname){
      $accession = $id;
      // because this is a relationship cvterm first check to see if it 
      // exists in the relationship ontology. If it does then return the cvterm.
      //  If not then set the dbname to _global and we'll add it or find it there
      $cvterm = db_fetch_object(db_query($cvtermsql,$name,'OBO_REL'));
      if($cvterm){
         return $cvterm;
      } else {
         // next check if this term is in the _global ontology.  If it is then
         // return it no matter what the original CV
         $dbname = '_global';

         $cvterm = db_fetch_object(db_query($cvtermsql,$name,$dbname));
         if($cvterm){
            return $cvterm;
         }
      }
   }
   if(!$is_relationship and !$dbname){
      tripal_cv_obo_quiterror("A database identifier is missing from the term: $id");
   }

   // check to see if the database exists. 
   $db = tripal_cv_obo_add_db($dbname);
   if(!$db){
      tripal_cv_obo_quiterror("Cannot find database '$dbname' in Chado.");
   }


   // if the cvterm doesn't exist then add it otherwise just update it
   $cvterm = db_fetch_object(db_query($cvtermsql,$name,$dbname));
   if(!$cvterm){
      // check to see if the dbxref exists if not, add it
      $dbxref =  tripal_cv_obo_add_dbxref($db->db_id,$accession);
      if(!$dbxref){
         tripal_cv_obo_quiterror("Failed to find or insert the dbxref record for cvterm, $name (id: $accession), for database $dbname");
      }

      // check to see if the dbxref already has an entry in the cvterm table
      $sql = "SELECT * FROM {cvterm} WHERE dbxref_id = %d";
      $check = db_fetch_object(db_query($sql,$dbxref->dbxref_id));

      if(!$check){
         // now add the cvterm
         $sql = "
            INSERT INTO {cvterm} (cv_id, name, definition, dbxref_id, 
               is_obsolete, is_relationshiptype) 
            VALUES (%d,'%s','%s',%d,%d,%d)
         ";
         if(!db_query($sql,$cv->cv_id,$name,$definition,
             $dbxref->dbxref_id,$is_obsolete,$is_relationship)){
            if(!$is_relationship){
               tripal_cv_obo_quiterror("Failed to insert the term: $name ($dbname)");
            } else {
               tripal_cv_obo_quiterror("Failed to insert the relationship term: $name (cv: " . $cvname . " db: $dbname)");
            }
         }  
         if(!$is_relationship){
            print "Added CV term: $name ($dbname)\n";
         } else {
            print "Added relationship CV term: $name ($dbname)\n";
         }
      }
      elseif($check and strcmp($check->name,$name)!=0){
         // this dbxref_id alrady exists in the database but the name is 
         // different.  We will trust that the OBO is correct and that there
         // has been a name change for this dbxref, so we'll update
         $sql = "
            UPDATE {cvterm} SET name = '%s', definition = '%s',
               is_obsolete = %d, is_relationshiptype = %d 
            WHERE dbxref_id = %d
         ";
         if(!db_query($sql,$name,$definition,$is_obsolete,$is_relationship,$dbxref->dbxref_id)){
            if(!$is_relationship){
               tripal_cv_obo_quiterror("Failed to update the term: $name ($dbname)");
            } else {
               tripal_cv_obo_quiterror("Failed to update the relationship term: $name (cv: " . $cvname . " db: $dbname)");
            }
         } 
         if(!$is_relationship){
            print "Updated CV term: $name ($dbname)\n";
         } else {
            print "Updated relationship CV term: $name ($dbname)\n";
         }
      }
      elseif($check and strcmp($check->name,$name)==0){
         // this entry already exists. We're good, so do nothing
      }
      $cvterm = db_fetch_object(db_query($cvtermsql,$name,$dbname));
   }
   elseif($update) { // update the cvterm
      $sql = "
         UPDATE {cvterm} SET name='%s', definition='%s',
            is_obsolete = %d, is_relationshiptype = %d
         WHERE cvterm_id = %d
      ";
      if(!db_query($sql,$term['name'][0],$definition,
          $is_obsolete,$is_relationship,$cvterm->cvterm_id)){
         tripal_cv_obo_quiterror("Failed to update the term: $name");
      }  
      $cvterm = db_fetch_object(db_query($cvtermsql,$name,$dbname));         
      if(!$is_relationship){
         print "Updated CV term: $name ($dbname)\n";
      } else {
         print "Updated relationship CV term: $name ($dbname)\n";
      }
   }
   // return the cvterm
   return $cvterm;
}

/**
*
* @ingroup tripal_obo_loader
*/
function tripal_cv_obo_add_dbxref($db_id,$accession,$version='',$description=''){

   // check to see if the dbxref exists if not, add it
   $dbxsql = "SELECT dbxref_id FROM {dbxref} WHERE db_id = %d and accession = '%s'";
   $dbxref = db_fetch_object(db_query($dbxsql,$db_id,$accession));
   if(!$dbxref){
      $sql = "
         INSERT INTO {dbxref} (db_id, accession, version, description)
         VALUES (%d,'%s','%s','%s')
      ";
      if(!db_query($sql,$db_id,$accession,$version,$description)){
         tripal_cv_obo_quiterror("Failed to insert the dbxref record $accession");
      }
      print "Added Dbxref accession: $accession\n";
      $dbxref = db_fetch_object(db_query($dbxsql,$db_id,$accession));
   }
   return $dbxref;

}

