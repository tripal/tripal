<?php

function tripal_core_load_gff3() {
   
   $gff_file = drupal_get_path('module', 'tripal_core').'/test.gff3';

   $lines = file($gff_file,FILE_SKIP_EMPTY_LINES);
   $i = 0;

   // get the controlled vocaubulary that we'll be using.  The
   // default is the 'sequence' ontology
   $vocab = 'sequence';
   $sql = "SELECT * FROM cv WHERE name = '%s'";
   $cv = db_fetch_object(db_query($sql,$vocab));

   foreach ($lines as $line_num => $line) {
      $i++;
      $cols = explode("\t",$line);
      if(sizeof($cols) > 9){
         print "ERROR: improper number of columns on line $i\n";
         return '';
      }
      // get the column values
      $seqid = $cols[0];
      $source = $cols[1];
      $type = $cols[2];
      $start = $cols[3];    
      $end = $cols[4];
      $score = $cols[5];
      $strand = $cols[6];
      $phase = $cols[7];
      $attrs = explode(";",$cols[8]);  // split by a semi-colon
      
      // break apart each of the attributes
      $tags = array();
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
      }

      // remove URL encoding

      // add the feature
      $sql = "INSERT INTO {feature} (organism_id, name, uniquename, residues, seqlen,".
             "    is_obsolete, type_id)".
             " VALUES(%d,'%s','%s','%s',%d, %s, ".
             "   (SELECT cvterm_id ".
             "    FROM {CVTerm} CVT ".
             "    INNER JOIN CV ON CVT.cv_id = CV.cv_id ".
             "    WHERE CV.name = 'sequence' and CVT.name = '%s'))";

   }
   return '';
}
