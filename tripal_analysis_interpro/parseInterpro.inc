<?php

/*******************************************************************************
 * Parse Interpro HTML Output file into analysisfeatureprop table
 */
function tripal_analysis_interpro_parseHTMLFile ($analysis_id, $interprofile, $parsego, $job_id) {

	// Prepare log
	$filename = preg_replace("/.*\/(.*)/", "$1", $interprofile);
	$logfile = file_directory_path() . "/tripal/tripal_analysis_interpro/load_$filename.log";
	$log = fopen($logfile, 'a'); // append parsing results to log file

	// Parsing started
	print "Parsing File:".$interprofile." ...\n";
	fwrite($log, date("D M j G:i:s Y").". Loading $interprofile\n");

	// Get cvterm_id for 'analysis_interpro_output_iteration_hits' which is required
	// for inserting into the analysisfeatureprop table
	$previous_db = tripal_db_set_active('chado'); // use chado database
	$sql = "SELECT CVT.cvterm_id FROM {cvterm} CVT ".
          "INNER JOIN cv ON cv.cv_id = CVT.cv_id ".
          "WHERE CVT.name = 'analysis_interpro_output_hit' ".
          "AND CV.name = 'tripal'";
	$type_id = db_result(db_query($sql));
	print "cvterm_id for analysis_interpro_output_iteration_hits is $type_id\n";

	// Load the HTML file and convert it into XML for loading
	$dom = new domDocument;
	$dom->loadHTMLFile($interprofile);
	$xml = $dom->saveXML();
	$interproput = simplexml_load_string($xml);

	// Get html tables for parsing
	$tables = $interproput->children()->children();

	// Count the number of tables to be processed
	$no_iterations = 0;
	foreach($tables as $tmp) {
		if ($tmp->getName() == 'table') {
			$no_iterations ++;
		}
	}
	print "$no_iterations html tables to be processed.\n";
	$interval = intval($no_iterations * 0.01);
	$idx_iterations = 0;

	// Processed the tables
	foreach ($tables as $table) {
        //if (preg_match('/No hits reported/', $table->asXML()) ) {
            //print "skipping this table b/c no hits are reported\n";
        //}
        // make sure we are looking at a table and its not an empty table
        if ($table->getName() == 'table' && !preg_match('/No hits reported/', $table->asXML()) ) {
            $idx_iterations ++;
            if ($idx_iterations % $interval == 0) {
                    $percentage = (int) ($idx_iterations / $no_iterations * 100);
                    tripal_db_set_active($previous_db);
                    tripal_job_set_progress($job_id, $percentage);
                    $previous_db = tripal_db_set_active('chado');
                    print $percentage."% ";
            }

            // Set job status
            // Get the first row and match its name with the feature name
            $firsttd = $table->children()->children()->children();
            $feature_id = 0;
            foreach($firsttd as $b) {
                foreach($b->children() as $a) {
                    if ($a->getName() == 'a') {
                        // Remove _ORF from the sequence name
                        $seqname = preg_replace('/^(.+?)_\d_.+/', "$1", $a);
                        print "seqname is $seqname\n";
    
                        // Find out how many features match this uniquename
                        $sql = "SELECT count(feature_id) FROM {feature} ".
                        "WHERE uniquename = '%s' ";
                        $no_features = db_result(db_query($sql, $seqname));
						    
                        // If there is only one match, get the feature_id
                        if ($no_features == 1) {
                            $sql = "SELECT feature_id FROM {feature} ".
                                    "WHERE uniquename = '%s' ";
                            $feature_id = db_result(db_query($sql, $seqname));
                            print "\tfeature id is $feature_id\n";
    
                            // If the uniquename matches more than one features then skip and print 'Ambiguous'
                        } else if ($no_features > 1) {
                            fwrite($log, "Ambiguous: ".$seqname." matches more than one feature and is not processed.\n");
                            continue;
    
                        // If the uniquename did not match, skip and print 'Failed'
                        } else {
                            fwrite($log, "Failed: ".$seqname."\n");
                        }
    
                    }
                }
            }

            // Successfully matched. print 'Succeeded'. Add analysis_id and
            // feature_id to analysisfeature. Add the table as XML to analysisfeatureprop
            if ($feature_id) {

                //------------------------------------
                // Clease unwanted rows from the table
                //------------------------------------

                $parent_row =   "/<tr><td valign=\"top\"><b>Parent<\/b><\/td>\s*<td valign=\"top\">\s*no.*?parent<\/td>\s*<\/tr>/";
                $children_row = "/<tr><td valign=\"top\"><b>Children<\/b><\/td>\s*<td valign=\"top\">\s*no.*?children<\/td>\s*<\/tr>/";
                $found_row    = "/<tr><td valign=\"top\"><b>Found.*?in<\/b><\/td>\s*<td valign=\"top\">\s*no.*?entries<\/td>\s*<\/tr>/";
                $contains_row = "/<tr><td valign=\"top\"><b>Contains<\/b><\/td>\s*<td valign=\"top\">\s*no.*?entries<\/td>\s*<\/tr>/";
                $go_row       = "/<tr><td valign=\"top\"><b>GO.*?terms<\/b><\/td>\s*<td valign=\"top\">\s*none<\/td>\s*<\/tr>/";
    
                $table_txt = $table->asXML();
                $table_txt = preg_replace($parent_row, "", $table_txt);
                $table_txt = preg_replace($children_row, "", $table_txt);
                $table_txt = preg_replace($found_row, "", $table_txt);
                $table_txt = preg_replace($contains_row, "", $table_txt);
                $table_txt = preg_replace($go_row, "", $table_txt);

                //------------------------------------
                // Clease unwanted ORF link from table 
                //------------------------------------
                $orf_link = "/<b><a href=\"\/iprscan\/wget.*?\">(.*?)<\/a><\/b>/";
                $table_txt = preg_replace($orf_link, "$1", $table_txt);

                //print "----------------------------\n";
                //print "old: ".$table->asXML()."\n\n\n";
                //print "----------------------------\n";
                //print "Fixed: $table_txt\n";
                //print "----------------------------\n";

                //------------------------------------
                // If this feature has already been associated with this analysis, do not reinsert
                // Otherwise, Insert into analysisfeature table
                //------------------------------------
                $sql = "Select analysisfeature_id as id from {analysisfeature} where feature_id = %d and analysis_id = %d";
                $analysisfeature =  db_fetch_object(db_query($sql, $feature_id, $analysis_id));
                if($analysisfeature){ $analysisfeature_id = $analysisfeature->id; }
                if(!$analysisfeature_id){
                    print "inserting analysisfeature\n";
                   $sql = "INSERT INTO {analysisfeature} (feature_id, analysis_id) ".
                        "VALUES (%d, %d)";
                   db_query ($sql, $feature_id, $analysis_id);
                   $sql = "Select analysisfeature_id from {analysisfeature} where feature_id = %d and analysis_id = %d";
                   $analysisfeature =  db_fetch_object(db_query($sql, $feature_id, $analysis_id));
                   $analysisfeature_id = $analysisfeature->id;
                }

                print "analysisfeature_id is $analysisfeature_id (analysis_id = $analysis_id; feature_id = $feature_id)\n";


                // Get the higest rank for this feature_id in analysisfeatureprop table.
                // If the value of the inserting content is not duplicate, add it to  
                // analysisfeaturepro with 'higest_rank + 1'
                $sql = "SELECT MAX(rank) FROM {analysisfeatureprop} AFP ".
                        "INNER JOIN analysisfeature AF ON AF.analysisfeature_id = AFP.analysisfeature_id ".       
                        "WHERE feature_id=%d ".
                        "AND analysis_id=%d ".
                        "AND type_id=%d ";

                $afp =  db_fetch_object(db_query($sql, $feature_id, $analysis_id, $type_id));
                $hi_rank = 0;
                if ($afp) {
                    $hi_rank = $afp->max + 1;
                }
            
				//------------------------------------------------------------
				// Insert interpro html tags into analysisfeatureprop table
				//------------------------------------------------------------
				// Before inserting, make sure it's not a duplicate
				$sql = "SELECT value FROM {analysisfeatureprop} WHERE analysisfeature_id = %d AND type_id = %d";
				$result = db_query($sql, $analysisfeature_id, $type_id);
				$duplicate = 0;
				while ($afp_value = db_fetch_object($result)) {
				    if ($table_txt == $afp_value->value) {
                        $duplicate = 1;
				    }
				}
				if (!$duplicate) {
				    $sql = "INSERT INTO {analysisfeatureprop} (analysisfeature_id, type_id, value, rank)".
                            "VALUES (%d, %d, '%s', %d)";
				    db_query($sql, $analysisfeature_id, $type_id, $table_txt, $hi_rank);
				    fwrite($log, " (Insert)\n"); // write to log
                    print "\twriting table\n";
				} else {
				   fwrite($log, " (Skipped)\n");
                    print "\tskipping table - dup\n";
				}
				
                // Parse GO terms. Make sure GO database schema is installed in chado
                $go_db_id = db_result(db_query("SELECT db_id FROM {db} WHERE name='GO'"));
                if (!$go_db_id) {
                    print 'GO schema not installed in chado. GO terms are not processed.';
                }
                if ($go_db_id && $parsego) {
                    $trs = $table->children();
                    foreach ($trs as $tr) {
                        $tds = $tr->children();
                        foreach($tds as $td) {
                            $gotags = $td->children();
                            foreach ($gotags as $gotag) {
                                // Look for 'GO:accession#'
                                if (preg_match("/^.*?GO:(\d+).*$/", $gotag, $matches)) {
                        	
                                    // Find cvterm_id for the matched GO term
                                    $sql = "SELECT cvterm_id FROM {cvterm} CVT 
                                            INNER JOIN dbxref DBX ON CVT.dbxref_id = DBX.dbxref_id
                                            WHERE DBX.accession = '%s' AND DBX.db_id = %d";
                                    $goterm_id = db_result(db_query($sql, $matches[1], $go_db_id));
                           
                                    //-------------------------------------------
                                    // Insert GO terms into feature_cvterm table
                                    //-------------------------------------------
                                    // Default pub_id = 1 (NULL) was used
                                    $sql = "INSERT INTO {feature_cvterm} (feature_id, cvterm_id, pub_id)
                                            VALUES (%d, %d, 1)";
                                    db_query($sql, $feature_id, $goterm_id);

                                    //------------------------------------------------
                                    // Insert GO terms into analysisfeatureprop table
                                    //------------------------------------------------
                                    $sql = "INSERT INTO {analysisfeatureprop} (analysisfeature_id, type_id, value, rank) ".
                                            "VALUES (%d, %d, '%s', 0)";
                                    db_query($sql, $analysisfeature_id, $goterm_id, $matches[1]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    tripal_db_set_active ($previous_db); // Use drupal database
    print "Done.\nSuccessful and failed entries have been saved in the log file:\n $logfile\n";
	
    fwrite($log, "\n");
    fclose($log);
    return;
}
/**
*
*/
function tripal_analysis_interpro_parseXMLFile ($analysis_id, $interproxmlfile, 
   $parsego, $query_re, $query_type, $query_uniquename, $job_id) 
{

   // clear out the anslysisfeature table for this analysis before getting started
   tripal_core_chado_delete('analysisfeature',array('analysis_id' => $analysis_id));

   // Prepare log
   $filename = preg_replace("/.*\/(.*)/", "$1", $interproxmlfile);
   $logfile = file_directory_path() . "/tripal/tripal_analysis_interpro/load_$filename.log";
   $log = fopen($logfile, 'a'); // append parsing results to log file

   // Parsing started
   print "Parsing File:".$interproxmlfile." ...\n";
   fwrite($log, date("D M j G:i:s Y").". Loading $interproxmlfile\n");

   // Get cvterm_id for 'analysis_interpro_xmloutput_hits' which is required
   // for inserting into the analysisfeatureprop table
   $previous_db = db_set_active('chado'); // use chado database
   $sql = "SELECT CVT.cvterm_id FROM {cvterm} CVT ".
   "INNER JOIN cv ON cv.cv_id = CVT.cv_id ".
   "WHERE CVT.name = 'analysis_interpro_xmloutput_hit' ".
   "AND CV.name = 'tripal'";
   $type_id = db_result(db_query($sql));

   // Load the XML file
   $interproput =  simplexml_load_file($interproxmlfile);

   // Get entries parsing
   $proteins = $interproput->children();

   // Count the number of entires to be processed
   $no_iterations = 0;
   foreach($proteins as $tmp) {
      $no_iterations ++;
   }
   print "$no_iterations proteins to be processed.\n";
   $interval = intval($no_iterations * 0.01);
   $idx_iterations = 0;

   // get the DB id for the GO database
   $parsego = tripal_analysis_get_property($analysis_id,'analysis_interpro_parsego');
   $go_db_id = db_result(db_query("SELECT db_id FROM {db} WHERE name='GO'"));
   if ($parsego and !$go_db_id) {
     print 'GO schema not installed in chado. GO terms are not processed.';;
   }

   // Processed each protein
   foreach ($proteins as $protein) {
      // Set job status
      $idx_iterations ++;
      if ($idx_iterations % $interval == 0) {
         $percentage = (int) ($idx_iterations / $no_iterations * 100);
         db_set_active($previous_db);
         tripal_job_set_progress($job_id, $percentage);
         $previous_db = db_set_active('chado');
         print $percentage."% ";
      }

      // match the protein id with the feature name
      $feature_id = 0;
      $attr = $protein->attributes();
      $seqname =$attr ['id'];

      // Remove _ORF from the sequence name
      $seqname = preg_replace('/^(.+)_\d+_ORF\d+.*/', '$1', $seqname);
     
      // if a regular expression is provided then pick out the portion requested
		if ($query_re and preg_match("/$query_re/", $seqname, $matches)) {
			$feature = $matches[1];
		} 
      // If no match by the regular expression then get everything up to the first space
      else {
			if (preg_match('/^(.*?)\s.*$/',  $seqname, $matches)) {
				$feature = $matches[1];
         } 
         // if no match up to the first space then just use the entire string
         else {
            $feature =  $seqname;
         }
      }   

      if(!$feature and $query_re){
         print fwrite($log, "Failed: Cannot find feature for '$seqname' using the regular expression: $query_re\n");
         continue;
      }

      // now find the feature in chado
      $select = array();
      if($query_uniquename){
         $select['uniquename'] = $feature;
      } else {
         $select['name'] = $feature;
      }
      if($query_type){
         $select['type_id'] = array(
           'cv_id' => array(
              'name' => 'sequence'
           ),
           'name' => $query_type,
         );
      }

      $feature_arr = tripal_core_chado_select('feature',array('feature_id'),$select);
      if(count($feature_arr) > 1){
		   fwrite($log, "Ambiguous: '$feature' matches more than one feature and is being skipped.\n");
			continue;
      }
      if(count($feature_arr) == 0){
			fwrite($log, "Failed: '$feature' cannot find a matching feature in the database.\n");
         continue;
      }
      $feature_id = $feature_arr[0]->feature_id;

     
      // Successfully matched. print 'Succeeded'. Add analysis_id and
      // feature_id to analysisfeature. Add the table as XML to analysisfeatureprop
      if ($feature_id) {

         print "$idx_iterations Adding InterPro results for feature '$seqname' ($feature_id)\n";

         // If a matched feature is found, write to log.
         fwrite($log, "Succeeded: ".$seqname." => feature id:".$feature_id);

         //------------------------------------
         // Insert into analysisfeature table
         //------------------------------------
         $sql = "INSERT INTO {analysisfeature} (feature_id, analysis_id) ".
                "VALUES (%d, %d)";
         db_query ($sql, $feature_id, $analysis_id);                     

         // Get the analysisfeature_id
         $sql = "SELECT analysisfeature_id FROM {analysisfeature} WHERE feature_id = %d AND analysis_id = %d";
         $analysisfeature_id = db_result(db_query($sql, $feature_id, $analysis_id));

         //------------------------------------------------------------
         // Insert interpro xml results into analysisfeatureprop table
         //------------------------------------------------------------
         // Check to see if we have an existing entry
         $sql = "SELECT analysisfeatureprop_id,rank 
                 FROM {analysisfeatureprop} 
                 WHERE analysisfeature_id = %d AND type_id = %d 
                 ORDER BY rank DESC";
         $result = db_fetch_object(db_query($sql, $analysisfeature_id, $type_id));
         $rank = 0;
         if($result){
            $afp_id = $result->analysisfeatureprop_id; 
            $rank = $result->rank + 1; 
         }
         $sql = "INSERT INTO {analysisfeatureprop} (analysisfeature_id, type_id, value, rank)".
                "VALUES (%d, %d, '%s', %d)";
         db_query($sql, $analysisfeature_id, $type_id, $protein->asXML(), $rank);
         fwrite($log, " (Insert)\n"); // write to log

         // parse the XML for each protein if GO terms are requested

         if($parsego and $go_db_id){		

            $protein = tripal_analysis_interpro_get_result_object($protein->asXML(),$feature_id); 
            $goterms = $protein['goterms'];                 

            // cycle through the GO terms and add them to the database
            foreach($goterms as $goterm){ 
                   
               // seperate the 'GO:' from the term
               if (preg_match("/^.*?GO:(\d+).*$/", $goterm, $matches)) {

                  // Find cvterm_id for the matched GO term
                  $sql = "SELECT cvterm_id FROM {cvterm} CVT 
                          INNER JOIN dbxref DBX ON CVT.dbxref_id = DBX.dbxref_id
                          WHERE DBX.accession = '%s' AND DBX.db_id = %d";
                  $goterm_id = db_result(db_query($sql, $matches[1], $go_db_id));

                  // Insert GO terms into feature_cvterm table
                  // Default pub_id = 1 (NULL) was used
                  $sql = "INSERT INTO {feature_cvterm} (feature_id, cvterm_id, pub_id)
                          VALUES (%d, %d, 1)";
                  db_query($sql, $feature_id, $goterm_id);


                  // Insert GO terms into analysisfeatureprop table
                  $sql = "INSERT INTO {analysisfeatureprop} (analysisfeature_id, type_id, value, rank) ".
                          "VALUES (%d, %d, '%s', 0)";
                  db_query($sql, $analysisfeature_id, $goterm_id, $matches[1]);
               } // end if preg_match
            } // end for each goterm
         } // end if($parsego and $go_db_id)
      } // end if($feature_id)            
   } // end foreach ($proteins as $protein)
   db_set_active ($previous_db); // Use drupal database
   print "Done.\nSuccessful and failed entries have been saved in the log file:\n $logfile\n";

   fwrite($log, "\n");
   fclose($log);
   return;
}
/********************************************************************************
 * 
 */
function tripal_analysis_interpro_get_result_object($interpro_xml,$feature_id){

	// Load the XML into an object
	$xmlObj = simplexml_load_string($interpro_xml);

   // iterate through each interpro results for this protein
   $results = array();
   $terms = array();
   $protein = array();
   $iprterms = array();
   $goterms = array();

   $term_count = 0;
   $match_count = 0;
   
   // get the properties of this result
   $attr = $xmlObj->attributes();
   $protein['orf_id'] = (string) $attr["id"];
   $protein['orf_length'] = (string) $attr["length"];
   $protein['orf_crc64'] = (string) $attr["crc64"];
   
	foreach($xmlObj->children() as $intepro){
      // get the interpro term for this match
      $attr = $intepro->attributes();
	   $terms[$term_count]['ipr_id'] = (string) $attr["id"];
	   $terms[$term_count]['ipr_name'] = (string) $attr["name"];
	   $terms[$term_count]['ipr_type'] = (string) $attr["type"];
      $iprterms[] = array($terms[$term_count]['ipr_id'],$terms[$term_count]['ipr_name']);
 
      // iterate through the elements of the interpro result
      $matches[$term_count]['matches'] = array();
      $match_count = 0;
  	   foreach($intepro->children() as $level1){
			$element_name = $level1->getName();
			if($element_name == 'match'){
            // get the match name for this match
            $attr = $level1->attributes();
	         $terms[$term_count]['matches'][$match_count]['match_id'] = (string) $attr["id"];
	         $terms[$term_count]['matches'][$match_count]['match_name'] = (string) $attr["name"];
	         $terms[$term_count]['matches'][$match_count]['match_dbname'] = (string) $attr["dbname"];
            
            // get the location information for this match     
            $loc_count = 0;
            foreach($level1->children() as $level2){
			      $element_name = $level2->getName();
			      if($element_name == 'location'){
                  $attr = $level2->attributes();
	               $terms[$term_count]['matches'][$match_count]['locations'][$loc_count]['match_start'] = (string) $attr["start"];
	               $terms[$term_count]['matches'][$match_count]['locations'][$loc_count]['match_end'] = (string) $attr["end"];
	               $terms[$term_count]['matches'][$match_count]['locations'][$loc_count]['match_score'] = (string) $attr["score"];
	               $terms[$term_count]['matches'][$match_count]['locations'][$loc_count]['match_status'] = (string) $attr["status"];
	               $terms[$term_count]['matches'][$match_count]['locations'][$loc_count]['match_evidence'] = (string) $attr["evidence"];
                  $loc_count++;
               }
            }
            $match_count++;
         }

			if($element_name == 'classification'){
            $attr = $level1->attributes();
            if($attr['class_type'] == 'GO'){
               $terms[$term_count]['matches'][$match_count]['go_terms'][] = (string) $attr['id'];
               $goterms[] = (string) $attr['id'];
            }
         }
      }
      $term_count++;
	}
   $results['terms'] = $terms;
   $results['orf'] = $protein;
   $results['iprterms'] = $iprterms;
   $results['goterms'] = $goterms;
	return $results;
}
