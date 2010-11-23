<?php
/*************************************************************************
*
*/
function tripal_core_load_obo_job (){
   global $user;

#   $file = 'ro.obo'; 
   $file = 'so_2_4_4.obo';
   $args = array($file);
   tripal_add_job("Load OBO $file",'tripal_core',
      "tripal_core_load_obo_v1_2",$args,$user->uid);

   return '';
}
/*************************************************************************
*
*/
function tripal_core_load_obo_v1_2($file) {

   $header = array();
   $obo = array();
   
   $obo_file = drupal_get_path('module', 'tripal_core')."/$file";
   print "Opening File $obo_file\n";

   // set the search path
   db_query("set search_path to chado,public");  // TODO: fix this

   // make sure we have an 'internal' and a '_global' database
   if(!tripal_core_obo_add_db('internal')){
      return tripal_core_obo_loader_done();
   }
   if(!tripal_core_obo_add_db('_global')){
      return tripal_core_obo_loader_done();
   }

   // parse the obo file
   tripal_core_obo_parse($obo_file,$obo,$header);

   // add the CV for this ontology to the database
   $cv = tripal_core_obo_add_cv($header['default-namespace'][0],'');
   if(!$cv){
      return tripal_core_obo_loader_done();
   }  

   // add any typedefs to the vocabulary first
   $typedefs = $obo['Typedef'];
   foreach($typedefs as $typedef){
      tripal_core_obo_add_cv_term($typedef,$cv,1);  
   }

   // next add terms to the vocabulary
   $terms = $obo['Term'];
   if(!tripal_core_obo_process_terms($terms,$cv,$obo)){
      return tripal_core_obo_loader_done();   
   }
   return tripal_core_obo_loader_done();
}

/*************************************************************************
*
*/
function tripal_core_obo_process_terms($terms,$cv,$obo){

   foreach ($terms as $term){

      // add the cvterm
      $cvterm = tripal_core_obo_add_cv_term($term,$cv);
      if(!$cvterm){ return 0; }
      if(isset($term['is_anonymous'])){
      }
      if(isset($term['alt_id'])){
      }
      if(isset($term['subset'])){
      }
      // add synonyms for this cvterm
      if(isset($term['synonym'])){
         if(!tripal_core_obo_add_synonyms($term,$cvterm)){
            return 0;
         }
      }
      if(isset($term['exact_synonym'])){
      }
      if(isset($term['narrow_synonym'])){
      }
      if(isset($term['broad_synonym'])){
      }
      if(isset($term['xref'])){
      }
      if(isset($term['xref_analog'])){
      }
      if(isset($term['xref_unk'])){
      }
      // add is_a relationships for this cvterm
      if(isset($term['is_a'])){
         foreach($term['is_a'] as $is_a){
            if(!tripal_core_obo_add_relationship($cvterm,$cv,$obo,'is_a',$is_a)){
               return 0;
            }
         }
      } 
      if(isset($term['intersection_of'])){
      }
      if(isset($term['union_of'])){
      }
      if(isset($term['disjoint_from'])){
      }
      if(isset($term['relationship'])){
         foreach($term['relationship'] as $value){
            $rel = preg_replace('/^(.+?)\s.+?$/','\1',$value);
            $object = preg_replace('/^.+?\s(.+?)$/','\1',$value);
            if(!tripal_core_obo_add_relationship($cvterm,$cv,$obo,$rel,$object)){
               return 0;
            }
         }
      }
      if(isset($term['replaced_by'])){
      }
      if(isset($term['consider'])){
      }
      if(isset($term['use_term'])){
      }
      if(isset($term['builtin'])){
      }
   }   
   return 1;
}
/*************************************************************************
*
*/
function tripal_core_obo_add_db($dbname){

   $db_sql = "SELECT * FROM {db} WHERE name ='%s'";
   $db = db_fetch_object(db_query($db_sql,$dbname));
   if(!$db){
      if(!db_query("INSERT INTO {db} (name) VALUES ('%s')",$dbname)){
         print "Cannot create '$dbname' db in Chado.";
         return 0;
      }      
     $db = db_fetch_object(db_query($db_sql,$dbname));
   }
   return $db;
}
/*************************************************************************
*
*/
function tripal_core_obo_add_cv($name,$comment){

 // see if the CV (default-namespace) exists already in the database
   $vocab = $name;
   $remark = $comment;
   $cv_sql = "SELECT * FROM {cv} WHERE name = '%s'";
   $cv = db_fetch_object(db_query($cv_sql,$vocab));

   // if the CV exists then update it, otherwise insert
   if(!$cv){
      $sql = "INSERT INTO {cv} (name,definition) VALUES ('%s','%s')";
      if(!db_query($sql,$vocab,$remark)){
         print "Failed to create the CV record";
         return 0;
      }
      $cv = db_fetch_object(db_query($cv_sql,$vocab));
   } else {
      $sql = "UPDATE {cv} SET definition = '%s' WHERE name ='%s'";
      if(!db_query($sql,$remark,$vocab)){
         print "Failed to update the CV record";
         return 0;
      }
      $cv = db_fetch_object(db_query($cv_sql,$vocab));
   }
   return $cv;
}
/*************************************************************************
*
*/
function tripal_core_obo_add_cvterm_prop($cvterm,$property,$value,$rank){

   // make sure the 'cvterm_property_type' CV exists
   $cv = tripal_core_obo_add_cv($property,'');
   if(!$cv){ return 0; }

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
      $cvproptype = tripal_core_obo_add_cv_term($term,$cv,0,0);
      if(!$cvproptype){  return 0; }      
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
      print "Could not add property $property for term\n";
      return 0;
   }
   return 1;
}
/*************************************************************************
*
*/
function tripal_core_obo_add_relationship($cvterm,$cv,$obo,$rel,$objname){

   // make sure the relationship cvterm exists
   $sql = "
        SELECT * 
        FROM {cvterm} CVT INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
        WHERE CVT.name = '%s' and CV.name = '%s'
   ";
   if(strcmp($rel,'is_a')==0){ // is_a is part of the OBO format and is in the 'relationship' ontology
      $cvisa = db_fetch_object(db_query($sql,$rel,'relationship'));
   } else {
      $cvisa = db_fetch_object(db_query($sql,$rel,$cv->name));
   }
   if(!$cvisa){
      print "Cannot find the relationship term: $rel\n";
      return 0;
   }

   // get the object term
   $objterm = tripal_core_obo_get_term($obo,$objname);
   if(!$objterm) { 
      print "Could not find object term $objname\n";
      return 0; 
   }
   $objcvterm = tripal_core_obo_add_cv_term($objterm,$cv);
   if(!$objcvterm){ return 0; }

   // check to see if the cvterm_relationship already exists, if not add it
   $cvrsql = "SELECT * FROM {cvterm_relationship} WHERE type_id = %d and subject_id = %d and object_id = %d";
   if(!db_fetch_object(db_query($cvrsql,$cvisa->cvterm_id,$cvterm->cvterm_id,$objcvterm->cvterm_id))){
      $sql = "INSERT INTO {cvterm_relationship} ".
             "(type_id,subject_id,object_id) VALUES (%d,%d,%d)";
      if(!db_query($sql,$cvisa->cvterm_id,$cvterm->cvterm_id,$objcvterm->cvterm_id)){
         print "Cannot add $rel relationship";
         return 0;
      }
//      print  "  $rel $objname\n";
   }

   return 1;
}
/*************************************************************************
*
*/
function tripal_core_obo_get_term($obo,$id){
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
/*************************************************************************
*
*/
function tripal_core_obo_add_synonyms($term,$cvterm){

   // make sure we have a 'synonym_type' vocabulary
   $sql = "SELECT * FROM {cv} WHERE name='synonym_type'";
   $syncv = db_fetch_object(db_query($sql));
   if(!$syncv){
      $sql = "INSERT INTO {cv} (name,definition) VALUES ('synonym_type','')";
      if(!db_query($sql)){

         print "Failed to add the synonyms type vocabulary";
         return 0;
      }
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
            if(!tripal_core_obo_add_cv_term($term,$syncv)){
               return 0;
            }
            $syntype = db_fetch_object(db_query($cvtsql,$type,'synonym_type'));
         }       

         // make sure the synonym doesn't already exists
         $sql = "
            SELECT * 
            FROM {cvtermsynonym} 
            WHERE cvterm_id = %d and synonym = '%s' and type_id = %d
         ";
         $syn = db_fetch_object(db_query($sql,$cvterm->cvterm_id,$def,$syntype->cvterm_id));
         if(!$syn){
            $sql = "INSERT INTO {cvtermsynonym} (cvterm_id,synonym,type_id)
                    VALUES(%d,'%s',%d)";
            if(!db_query($sql,$cvterm->cvterm_id,$def,$syntype->cvterm_id)){
               print "Failed to insert the synonym for term: $name ($def)\n";
               return 0;
            }
         }            
      } 
   }
   return 1;
}
/*************************************************************************
*
*/
function tripal_core_obo_add_cv_term($term,$cv,$is_relationship = 0,$update = 1){

   // get the term properties
   $name = $term['name'][0];
   $definition = preg_replace('/^\"(.*)\"/','\1',$term['def'][0]);
   $is_obsolete = 0;
   if(isset($term['is_obsolete'][0]) and  strcmp($term['is_obsolete'][0],'true')==0){
     $is_obsolete = 1;
   }

   // get the accession and the database from the cvterm
   $accession = preg_replace('/^.+?:(.*)$/','\1',$term['id'][0]);
   $db = preg_replace('/^(.+?):.*$/','\1',$term['id'][0]);

   // check to see if the database exists
   $db = tripal_core_obo_add_db($db);
   if(!$db){
      print "Cannot find database '$db' in Chado.";
      return 0;
   }

   // check to see if the cvterm already exists
   $cvtermsql = "SELECT * from {cvterm} WHERE name = '%s' and cv_id = %d";
   $cvterm = db_fetch_object(db_query($cvtermsql,$name,$cv->cv_id));

   // if the cvterm doesn't exist then add it otherwise just update it
   if(!$cvterm){
      // check to see if the dbxref exists if not, add it
      $dbxsql =  tripal_core_obo_add_dbxref($db->db_id,$accession);
      if(!$dbxref){
         print "Failed to find or insert the dbxref record for cvterm: $name ($accession)";
         return 0;
      }

      // now add the cvterm
      $sql = "
         INSERT INTO {cvterm} (cv_id, name, definition, dbxref_id, 
            is_obsolete, is_relationshiptype) 
         VALUES (%d,'%s','%s',%d,%d,%d)
      ";
      if(!db_query($sql,$cv->cv_id,$name,$definition,
          $dbxref->dbxref_id,$is_obsolete,$is_relationship)){
         print "Failed to insert the term: " . $term['name'][0];
         return 0;
      }     
      print "Added CV term: $name\n";
      $cvterm = db_fetch_object(db_query($cvtermsql,$name,$cv->cv_id));
   }
   elseif($update) { // update the cvterm
      $sql = "
         UPDATE {cvterm} SET name='%s', definition='%s',
            is_obsolete = %d, is_relationshiptype = %d
         WHERE cvterm_id = %d
      ";
      if(!db_query($sql,$term['name'][0],$definition,
          $is_obsolete,$is_relationship,$cvterm->cvterm_id)){
         print "Failed to update the term: $name\n";
         return 0;
      }  
      print "Updated CV term: $name\n";
      $cvterm = db_fetch_object(db_query($cvtermsql,$name,$cv->cv_id));         
   }

   // add the comment to the cvtermprop table
   if(isset($term['comment'])){
      $comments = $term['comment'];
      $j = 0;
      foreach($comments as $comment){
         if(!tripal_core_obo_add_cvterm_prop($cvterm,'comment',$comment,$j)){
            return 0;
         }
         $j++;
      }
   }
 

   // add any other external dbxrefs
   if(isset($term['xref'])){
      foreach($term['xref'] as $xref){
         $accession = preg_replace('/^.+?:(.*)$/','\1',$xref);
         $dbname = preg_replace('/^(.+?):.*$/','\1',$xref);

         // if the xref is a database link, handle that specially
         if(strcmp($db,'http')==0){
            $accession = $xref;
            $dbname = 'URL';
         }

         // check to see if the database exists
         $db = tripal_core_obo_add_db($db);
         if(!$db){
            print "Cannot find database '$db' in Chado.";
            return 0;
         }

         // now add the dbxref
         $dbxref = tripal_core_obo_add_dbxref($db->db_id,$accession);
         if(!$dbxref){ return 0;}

         // finally add the cvterm_dbxref but first check to make sure it exists
         $sql = "SELECT * from {cvterm_dbxref} WHERE cvterm_id = %d and dbxref_id = %d";
         if(!db_fetch_object(db_query($sql,$cvterm->cvterm_id,$dbxref->dbxref_id))){            
            $sql = "INSERT INTO {cvterm_dbxref} (cvterm_id,dbxref_id)".
                   "VALUES (%d,%d)";
            if(!db_query($sql,$cvterm->cvterm_id,$dbxref->dbxref_id)){
               print "Cannot add cvterm_dbxref: $accession\n";
               return 0;
            }
         }
      }
   }

   // return the cvterm
   return $cvterm;
}
/*************************************************************************
*
*/
function tripal_core_obo_add_dbxref($db_id,$accession,$version='',$description=''){

   // check to see if the dbxref exists if not, add it
   $dbxsql = "SELECT dbxref_id FROM {dbxref} WHERE db_id = %d and accession = '%s'";
   $dbxref = db_fetch_object(db_query($dbxsql,$db_id,$accession));
   if(!$dbxref){
      $sql = "
         INSERT INTO {dbxref} (db_id, accession, version, description)
         VALUES (%d,'%s','%s','%s')
      ";
      if(!db_query($sql,$db_id,$accession,$version,$description)){
         print "Failed to insert the dbxref record $accession\n";
         return 0;
      }
      $dbxref = db_fetch_object(db_query($dbxsql,$db_id,$accession));
   }
   return $dbxref;

}
/*************************************************************************
*
*/
function tripal_core_obo_parse($obo_file,&$obo,&$header){
   $i = 0;
   $in_header = 1;
   $stanza = array();

   $lines = file($obo_file,FILE_SKIP_EMPTY_LINES);

   // iterate through the lines in the OBO file and parse the stanzas
   foreach ($lines as $line_num => $line) {
      $i++;

      // remove newlines
      $line = rtrim($line);  
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

/*************************************************************************
*
*/
function tripal_core_obo_loader_done (){
   // return the search path to normal
   db_query("set search_path to public");  
   return '';
}
