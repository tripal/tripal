<?php

/*******************************************************************************
 * Parse NCBI Blast results for indexing so that user can use blast results to
 * find corresponding features
 */
function parse_NCBI_Blast_XML_index_version($xml_string,$db,$feature_id) {
	// Get the parser using db_id
	$sql = "SELECT * FROM {tripal_analysis_blast} WHERE db_id = %d";
	$parser = db_fetch_object(db_query($sql, $db->db_id));
	$db_name = $parser->displayname;
	$is_genbank = $parser->genbank_style;
	$regex_hit_id = $parser->regex_hit_id;
	$regex_hit_def = $parser->regex_hit_def;
	$regex_hit_accession = $parser->regex_hit_accession;

	// set default if regular expressions have not been specified
	if(!$regex_hit_id){
		$regex_hit_id = '/^(.*?)\s.*$/';
	} else {
		$regex_hit_id = '/'.$regex_hit_id.'/';
	}
	if(!$regex_hit_def){
		$regex_hit_def = '/^.*?\s(.*)$/';
	} else {
		$regex_hit_def = '/'.$regex_hit_def.'/';
	}
	if(!$regex_hit_accession){
		$regex_hit_accession = '/^(.*?)\s.*$/';
	} else {
		$regex_hit_accession = '/'.$regex_hit_accession.'/';
	}

	$html_out .= "<h3>$db_name</h3>";

	// Load the file.  This XML file should be an extract
	// of the original XML file with only a single iteration.
	// An iteration is essentially all the hits for a single
	// query sequence.
	$xml_output = simplexml_load_string($xml_string);
	$iteration = '';
	// new XML file parser has added the feature name within <Iteration_query-def> tags.
	if ($xml_output->getName() == 'Iteration') {
		foreach ($xml_output->children() as $xml_tag) {
			if ($xml_tag->getName() == 'Iteration_query-def') {
				// Here we show the feature name again to check if we pull the correct data
				$html_out .= "Query: $xml_tag<br>";
			} else if ($xml_tag->getName() == 'Iteration_hits') {
				$iteration = $xml_tag;
			}
		}
		// This is for the file parsed by the old parser
	} else {
		$iteration = $xml_output;
	}

	// now run through the blast hits/hsps of this iteration
	// and generate the rows of the table
	foreach($iteration->children() as $hits){
		$best_evalue = 0;
		foreach($hits->children() as $hit){
			$best_evalue = 0;
			$element_name = $hit->getName();
			if($element_name == 'Hit_id'){
				// if parsing "name, acc, desc" from three tags (1/3)
				if ($is_genbank) {
					$hit_name = $hit;
				}
			} else if($element_name == 'Hit_def'){
				if($is_genbank){
					$description = $hit;
				} else {
					$accession = preg_replace($regex_hit_accession,"$1",$hit);
					$hit_name = preg_replace($regex_hit_id,"$1",$hit);
					$description = preg_replace($regex_hit_def,"$1",$hit);
				}
			} else if($element_name == 'Hit_accession'){
				// if parsing "name, acc, desc" from three tags (3/3)
				if ($is_genbank){
					$accession = $hit;
				}
				// now run through each HSP for this hit
			}
		}
		$html_out .= "<p>$hit_name<br>";
		$html_out .= "$accession<br>";
		$html_out .= "<b>$description</b></br>";
		$hsp_html_out = '';
	}
	return $html_out;
}

/*******************************************************************************
 * Parse Blast XML Output file into analysisfeatureprop table
 */
function tripal_analysis_blast_parseXMLFile ($analysis_id, $blastdb, $blastfile, 
   $no_parsed, $blastfile_ext, $query_re, $query_type, $query_uniquename,
   $is_concat,$job_id,$is_concat) {

	// Prepare log
	$filename = preg_replace("/.*\/(.*)/", "$1", $blastfile);
	$logfile = file_directory_path() . "/tripal/tripal_analysis_blast/load_$filename.log";
        $logfile = tempnam(sys_get_temp_dir(),"tripal_analysis_blast_import");
	$log = fopen($logfile, 'a'); // append parsing results to log file
        if(!$log){
           print "ERROR: cannot open log file: $logfile\n";
           exit;
        }

	// If user input a file (e.g. blast.xml)
	if (is_file($blastfile)) {
      tripal_analysis_blast_parseXML($analysis_id, $blastdb, $blastfile, 
         $no_parsed, $blastfile_ext, $query_re, $query_type, $query_uniquename,
         $job_id,1,$log,$is_concat);
	} 
   // Otherwise, $blastfile is a directory. Iterate through all xml files in it
   else {
      if(!$blastfile_ext){
         $blastfile_ext = 'xml';
      }

		$dir_handle = @opendir($blastfile) or die("Unable to open $blastfile");
		$pattern = sql_regcase($blastfile . "/*.$blastfile_ext");
		$total_files = count(glob($pattern));
		print "$total_files file(s) to be parsed.\n";
		
		$interval = intval($total_files * 0.01);
		$no_file = 0;
			
		// Parsing all files in the directory
		while ($file = readdir($dir_handle)) {
			if(preg_match("/^.*\.$blastfile_ext/i",$file)){

            tripal_analysis_blast_parseXML($analysis_id, $blastdb, "$blastfile/$file", 
              $no_parsed, $blastfile_ext, $query_re, $query_type, $query_uniquename,
              $job_id,0,$log);

			   // Set job status
			   if ($no_file % $interval == 0) {
			      $percentage = (int) (($no_file / $total_files) * 100);
				   tripal_job_set_progress($job_id, $percentage);
				   print $percentage."%\r";
			   }				
         }
   	   $no_file ++;
		}
	}

	print "Done.\nSuccessful and failed entries have been saved in the log file:\n $logfile\n";
	fwrite($log, "\n");
	fclose($log);
	return;
}
/********************************************************************************
 * 
 */
function tripal_analysis_blast_parseXML($analysis_id, $blastdb, $blastfile, 
   $no_parsed, $blastfile_ext, $query_re, $query_type, $query_uniquename,
  $job_id,$set_progress,$log,$is_concat){

	// Parsing started
	print "Parsing File:".$blastfile." ...\n";
	fwrite($log, date("D M j G:i:s Y").". Loading $blastfile\n");
	if ($no_parsed == 'all') {
		print "Parsing all hits...\n";
	} else {
		print "Parsing top $no_parsed hits...\n";
	}
	
	// Get cvterm_id for 'analysis_blast_output_iteration_hits' which is required
	// for inserting into the analysisfeatureprop table
	$previous_db = tripal_db_set_active('chado'); // use chado database
	$sql = "SELECT CVT.cvterm_id ".
          "FROM {cvterm} CVT ".
          "   INNER JOIN {cv} ON cv.cv_id = CVT.cv_id ".
          "WHERE CVT.name = 'analysis_blast_output_iteration_hits' ".
          "   AND CV.name = 'tripal'";
	$type_id = db_result(db_query($sql));

	// Load the XML file.
   if (!is_readable($blastfile)) {
      exit("Could not open the XML file '$blastfile'.  Check that file exists and that permissions are correct.\n");
   }
   
   // if the file is a set of concatenated files then we want to split it up
   // and run each one individually
   if($is_concat){

      // generate a temporary file name
      $temp = tempnam(sys_get_temp_dir(),'blast_');
      print "Blast XML file is concatenated.  Breaking apart and parsing each individually: $temp\n";
      $out_fh = fopen($temp,"w");

      // run through the lines of the XML file
      $in_fh = fopen($blastfile,"r");
      while(!feof($in_fh)){
         $line = fgets($in_fh);
         $line = trim($line);
         if(!$line){
            continue;
         }
         fwrite($out_fh,"$line\n");
         // if the line begins a set of blast output XML then parse the
         // preceeding set.
         if(preg_match("/<\/BlastOutput>/",$line)){
            // close the temp file
            fclose($out_fh);
            // now parse this new temp file
            tripal_analysis_blast_parseXML($analysis_id, $blastdb, $temp, 
               $no_parsed, $blastfile_ext, $query_re, $query_type, $query_uniquename,
               $job_id,$set_progress,$log,0);
            // reopen the file for the next set of results
            $out_fh = fopen($temp,"w");
         }
      }
      fclose($in_fh);

      return;
   } 

	$blastoutput = simplexml_load_file($blastfile);
   if(!$blastoutput){
      exit("Could not read the XML file '$blastfile'.  Check that the XML file is not corrupted.\n");
   }
	$no_iterations = 0;
	foreach($blastoutput->children() as $tmp) {
		if ($tmp->getName() == 'BlastOutput_iterations') {
			foreach($tmp->children() as $itr) {
				if ($itr->getName() == 'Iteration') {
					$no_iterations ++;
				}
			}
		}
	}
	print "$no_iterations iterations to be processed.\n";
		
	$interval = intval($no_iterations * 0.01);
	$idx_iterations = 0;
	foreach ($blastoutput->children() as $blastoutput_tags) {

		if ($blastoutput_tags->getName() == 'BlastOutput_iterations') {
			foreach($blastoutput_tags->children() as $iterations) {
				if ($iterations->getName() == 'Iteration') {

					// Set job status
					$idx_iterations ++;
					if ($set_progress and $idx_iterations % $interval == 0) {
						$percentage = (int) (($idx_iterations / $no_iterations) * 100);
						tripal_job_set_progress($job_id, $percentage);
						print $percentage."%\r";
					}
					// now run through the blast hits/hsps of this iteration
					// and generate the rows of the table
					$feature_id = 0;
					foreach($iterations->children() as $iteration_tags) {

						// Match chado feature uniquename with <Iteration_query-def>
						// and get the feature_id
						$featurenaem_xml = '';
						if($iteration_tags->getName() == 'Iteration_query-def'){

							// If the Iteration_query-def in the format provided by the 
                     // user's regular expression 
							if ($query_re and preg_match("/$query_re/", $iteration_tags, $matches)) {
								$feature = $matches[1];
							} 
                     // If not in above format then pull up to the first space
                     else {
								if (preg_match('/^(.*?)\s.*$/', $iteration_tags, $matches)) {
   								$feature = $matches[1];
                        } 
                        // if no match up to the first space then just use the entire string
                        else {
                           $feature = $iteration_tags;
                        }
                     }   

                     if(!$feature and $query_re){
                        print "ERROR: cannot find feature for $iteration_tags using the regular expression: $query_re\n";
                        exit;
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

    						fwrite($log, "Matched: '$feature' => feature id:".$feature_id);
							$featurename_xml = $iteration_tags->asXML();
						} 
                  // Insert Iteration_hits into analysisfeatureprop and analysisfeature tables
                  else if($iteration_tags->getName() == 'Iteration_hits'){
							if ($feature_id) {
								// Make sure this iteration doesn't exist in analysisfeatureprop. If it does, update but not insert
								$sql = "SELECT analysisfeatureprop_id FROM {analysisfeatureprop} AFP ".
                               "INNER JOIN analysisfeature AF ON AF.analysisfeature_id = AFP.analysisfeature_id ".       
                               "WHERE feature_id=%d ".
                            "AND analysis_id=%d ".
                            "AND type_id=%d ";

								$result = db_query($sql, $feature_id, $analysis_id, $type_id);
								$analysisfeatureprop = db_fetch_object($result);
								$xml_content =  "<Iteration>\n".$featurename_xml."\n";
								// parse all hits
								if ($no_parsed == 'all') {
									$xml_content .= $iteration_tags->asXML();
								// parse only top hits
								} else {
									$counter = 0;
									$xml_content .= "<Iteration_hits>\n";
									foreach ($iteration_tags->children() As $hit) {
										if ($counter < $no_parsed) {
											$xml_content .= $hit->asXML();
										} else {
											break;
										}
										$counter ++;
									}
									$xml_content .= "</Iteration_hits>";
								}
								$xml_content .= "\n</Iteration>";

								// If this Iteration_hits already exists, update it
								if ($analysisfeatureprop) {

									$sql = "UPDATE {analysisfeatureprop} ".
                               "SET value = '%s' ".
                               "WHERE analysisfeatureprop_id = %d ";
										
									db_query($sql, $xml_content, $analysisfeatureprop->analysisfeatureprop_id);
									fwrite($log, " (Update)\n"); // write to log

									// Otherwise, insert the Iteration_hits into analysisfeature and analysisfeatureprop tables
								} else {
									//------------------------------------------------------
									// Insert into analysisfeature table
									//------------------------------------------------------
									$sql = "INSERT INTO {analysisfeature} (feature_id, analysis_id) ".
                               "VALUES (%d, %d)";
									db_query ($sql, $feature_id, $analysis_id);
									// Get the newly inserted analysisfeature_id
									$sql = "SELECT analysisfeature_id FROM {analysisfeature} WHERE feature_id = %d AND analysis_id = %d";
									$analysisfeature_id = db_result(db_query($sql, $feature_id, $analysis_id));
									//------------------------------------------------------
									// Insert into analysisfeatureprop table
									//------------------------------------------------------
									$sql = "INSERT INTO {analysisfeatureprop} (analysisfeature_id, type_id, value, rank)".
                               "VALUES (%d, %d, '%s', %d)";
									db_query($sql, $analysisfeature_id, $type_id, $xml_content, '0');
									fwrite($log, " (Insert)\n"); // write to log
								}
							}
						}	
					}
				}
			}
		}
	}
	tripal_db_set_active ($previous_db); // Use drupal database
}
/********************************************************************************
 * 
 */
function tripal_analysis_blast_get_result_object($xml_string,$db,$max,$feature_id, $analysis) {
	$blast_object = new stdClass();

	// Get the parser using db_id
	$sql = "SELECT * FROM {tripal_analysis_blast} WHERE db_id = %d";
	$parser = db_fetch_object(db_query($sql, $db->db_id));
	$db_name = $parser->displayname;
	$is_genbank = $parser->genbank_style;
	$regex_hit_id = $parser->regex_hit_id;
	$regex_hit_def = $parser->regex_hit_def;
	$regex_hit_accession = $parser->regex_hit_accession;

	// set default if regular expressions have not been specified
	if(!$regex_hit_id){
		$regex_hit_id = '/^(.*?)\s.*$/';
	} else {
		$regex_hit_id = '/'.$regex_hit_id.'/';
	}
	if(!$regex_hit_def){
		$regex_hit_def = '/^.*?\s(.*)$/';
	} else {
		$regex_hit_def = '/'.$regex_hit_def.'/';
	}
	if(!$regex_hit_accession){
		$regex_hit_accession = '/^(.*?)\s.*$/';
	} else {
		$regex_hit_accession = '/'.$regex_hit_accession.'/';
	}

	// Get analysis information
	$blast_object->analysis = $analysis;
	$db->displayname = $db_name;
	$blast_object->db = $db;
	if (!$db_name) {
		$blast_object->title = $analysis->name;
	} else {
		$blast_object->title = $db_name;
	}

	// Find node id for the analysis
	$ana_nid = db_result(db_query("SELECT nid FROM {chado_analysis} WHERE analysis_id = %d", $analysis->analysis_id));
   $analysis->nid = $ana_nid;


	// Load the file.  This XML file should be an extract
	// of the original XML file with only a single iteration.
	// An iteration is essentially all the hits for a single
	// query sequence.
	$xml_output = simplexml_load_string($xml_string);
	$iteration = '';
	// new XML file parser has added the feature name within <Iteration_query-def> tags.
	if ($xml_output->getName() == 'Iteration') {
		foreach ($xml_output->children() as $xml_tag) {
			if ($xml_tag->getName() == 'Iteration_query-def') {
				// Here we show the feature name again to check if we pull the correct data
				$blast_object->xml_tag = $xml_tag;
			} else if ($xml_tag->getName() == 'Iteration_hits') {
				$iteration = $xml_tag;
			}
		}
		// This is for the file parsed by the old parser
	} else {
		$iteration = $xml_output;
	}

	$number_hits = 0;
	foreach($iteration->children() as $hits){
		$number_hits ++;
	}

	// add the links for updating blast info using Ajax
	$blast_object->max = $max;
	$blast_object->number_hits = $number_hits;
	$blast_object->feature_id = $feature_id;
	
	$hits_array = array();
	$hit_count = 0;

	foreach($iteration->children() as $hits){
		$hsp_array = array();
		$counter = 0;
		foreach($hits->children() as $hit){
			$best_evalue = 0;
			$best_identity = 0;
			$best_len = 0;
			$element_name = $hit->getName();
			if($element_name == 'Hit_id'){
				// if parsing "name, acc, desc" from three tags (1/3)
				if ($is_genbank) {
					$hit_name = $hit;
				}
			} else if($element_name == 'Hit_def'){
				if($is_genbank){
					$description = $hit;
				} else {
					$accession = preg_replace($regex_hit_accession,"$1",$hit);
					$hit_name = preg_replace($regex_hit_id,"$1",$hit);
					$description = preg_replace($regex_hit_def,"$1",$hit);
				}
			} else if($element_name == 'Hit_accession'){
				// if parsing "name, acc, desc" from three tags (3/3)
				if ($is_genbank){
					$accession = $hit;
				}
				// now run through each HSP for this hit
			} else if($element_name == 'Hit_hsps'){		
				foreach($hit->children() as $hsp){
					foreach($hsp->children() as $hsp_info){
						$element_name = $hsp_info->getName();
						if($element_name == 'Hsp_num'){
							$hsp_num = $hsp_info;
						}
						if($element_name == 'Hsp_bit-score'){
							$hsp_bit_score = $hsp_info;
						}
						if($element_name == 'Hsp_score'){
							$hsp_score = $hsp_info;
						}
						if($element_name == 'Hsp_evalue'){
							$hsp_evalue = $hsp_info;
							// use the first evalue for this set of HSPs
							// as the best evalue. This get's shown as
							// info for the overall match.
							if(!$best_evalue){
								$best_evalue = $hsp_evalue;
							}
						}
						if($element_name == 'Hsp_query-from'){
							$hsp_query_from = $hsp_info;
						}
						if($element_name == 'Hsp_query-to'){
							$hsp_query_to = $hsp_info;
						}
						if($element_name == 'Hsp_hit-from'){
							$hsp_hit_from = $hsp_info;
						}
						if($element_name == 'Hsp_hit-to'){
							$hsp_hit_to = $hsp_info;
						}
						if($element_name == 'Hsp_query-frame'){
							$hsp_query_frame = $hsp_info;
						}
						if($element_name == 'Hsp_identity'){
							$hsp_identity = $hsp_info;
							// use the first evalue for this set of HSPs
							// as the best evalue. This get's shown as
							// info for the overall match.
							if(!$best_identity){
								$best_identity = $hsp_identity;
							}
						}
						if($element_name == 'Hsp_positive'){
							$hsp_positive = $hsp_info;
						}
						if($element_name == 'Hsp_align-len'){
							$hsp_align_len = $hsp_info;
							// use the first evalue for this set of HSPs
							// as the best evalue. This get's shown as
							// info for the overall match.
							if(!$best_len){
								$best_len = $hsp_align_len;
							}
						}
						if($element_name == 'Hsp_qseq'){
							$hsp_qseq = $hsp_info;
						}
						if($element_name == 'Hsp_hseq'){
							$hsp_hseq = $hsp_info;
						}
						if($element_name == 'Hsp_midline'){
							$hsp_midline = $hsp_info;
						}
					}

					$hsp_content = array();
					$hsp_content['hsp_num'] = $hsp_num;
					$hsp_content['bit_score'] = $hsp_bit_score;
					$hsp_content['score'] = $hsp_score;
					$hsp_content['evalue'] = $hsp_evalue;
					$hsp_content['query_frame'] = $hsp_query_frame;
					$hsp_content['qseq'] = $hsp_qseq;
					$hsp_content['midline'] = $hsp_midline;
					$hsp_content['hseq'] = $hsp_hseq;
					$hsp_content['hit_from'] = $hsp_hit_from;
					$hsp_content['hit_to'] = $hsp_hit_to;
					$hsp_content['identity'] = $hsp_identity;
					$hsp_content['align_len'] = $hsp_align_len;
					$hsp_content['positive'] = $hsp_positive;
					$hsp_content['query_from'] = $hsp_query_from;
					$hsp_content['query_to'] = $hsp_query_to;
					$hsp_array[$counter] = $hsp_content;
					$counter ++;
				}
			}
		}
      $arrowr_url = url(drupal_get_path('theme', 'tripal')."/images/arrow_r.png");
		$hits_array[$hit_count]['arrowr_url'] = $arrowr_url;
		$hits_array[$hit_count]['accession'] = $accession;
		$hits_array[$hit_count]['hit_name'] = $hit_name;
		
		if($accession && $db->urlprefix){
			$hits_array[$hit_count]['hit_url'] = "$db->urlprefix$accession";
		} else {
			// Test if this is another feature in the database
			$sql = "SELECT feature_id FROM {feature} WHERE uniquename = '%s'";
			$previous_db = db_set_active('chado');
			$hit_feature_id = db_result(db_query($sql, $hit_name));
			db_set_active($previous_db);
			// If it is, add link to that feature
			if ($hit_feature_id) {
				$hits_array[$hit_count]['hit_url'] = "ID$hit_feature_id";
			}
		}
		
		$hits_array[$hit_count]['best_evalue'] = $best_evalue;
		$percent_identity = number_format($best_identity/$best_len*100, 2);
		$hits_array[$hit_count]['percent_identity'] = $percent_identity;
		$hits_array[$hit_count]['description'] = $description;
		$hits_array[$hit_count]['hsp'] = $hsp_array;
		$hit_count ++;
		
		// if we've hit the maximum number of hits then return
		if($max > 0 && $hit_count >= $max){
			break;
		}
	}
	$blast_object->hits_array = $hits_array;
	return $blast_object;
}

/********************************************************************************
 * Parse the best hit to generate the best hit homology report 
 */
function tripal_analysis_blast_parse_best_hit ($analysis_id) {

	// Select all features for this blast analysis, and save them to the 'featureSet' array
	$sql = "SELECT feature_id
	             FROM {analysisfeature} AF  
	             WHERE analysis_id = %d";
	$previous_db = tripal_db_set_active('chado');
	$result = db_query($sql, $analysis_id);
	$featureSet = array ();
	$counter = 0;
	while ($feature = db_fetch_object($result)) {
		$featureSet [$counter] = $feature->feature_id;
		$counter ++;
	}

	// Get analysis information including 'Time', 'Name', and 'DB Settings'
	$sql = "SELECT value, name, to_char(timeexecuted, 'MM-DD-YYYY') AS time
                FROM {analysis}  A
                INNER JOIN {analysisprop} AP ON  A.analysis_id = AP.analysis_id 
                WHERE A.analysis_id = %d
                AND type_id= (SELECT cvterm_id 
                                           FROM {cvterm} 
                                           WHERE name = 'analysis_blast_settings')";
	$analysis = db_fetch_object(db_query($sql, $analysis_id));
	
	// Parse the blast settings
	$blastsettings = explode("|", $analysis->value);
	$db_id = $blastsettings [0];
	
	// Get the xml description parser using db_id
	tripal_db_set_active($previous_db);
	$sql = "SELECT * FROM {tripal_analysis_blast} WHERE db_id = %d";
	$parser = db_fetch_object(db_query($sql, $db_id));

	$db_name = $parser->displayname;
	$is_genbank = $parser->genbank_style;
	$regex_hit_id = $parser->regex_hit_id;
	$regex_hit_def = $parser->regex_hit_def;
	$regex_hit_accession = $parser->regex_hit_accession;

	// set default description parser  if regular expressions have not been specified
	if(!$regex_hit_id){
		$regex_hit_id = '/^(.*?)\s.*$/';
	} else {
		$regex_hit_id = '/'.$regex_hit_id.'/';
	}
	if(!$regex_hit_def){
		$regex_hit_def = '/^.*?\s(.*)$/';
	} else {
		$regex_hit_def = '/'.$regex_hit_def.'/';
	}
	if(!$regex_hit_accession){
		$regex_hit_accession = '/^(.*?)\s.*$/';
	} else {
		$regex_hit_accession = '/'.$regex_hit_accession.'/';
	}
	
   $interval = intval($counter * 0.01);
	for ($i = 0; $i < $counter; $i ++) {
		
		if ($i !=0 && $i % $interval == 0) {
			$percentage = (int) ($i / $counter * 100);
			tripal_job_set_progress($job_id, $percentage);	
			print $percentage."%\r";
		}

		$sql = "SELECT value
		             FROM {analysisfeatureprop} AFP 
		             INNER JOIN {analysisfeature} AF ON AFP.analysisfeature_id = AF.analysisfeature_id 
		             WHERE analysis_id = %d 
		             AND feature_id = %d
		             AND type_id = (SELECT cvterm_id FROM cvterm WHERE name='analysis_blast_output_iteration_hits' AND cv_id = (SELECT cv_id FROM cv WHERE name='tripal'))";
		$previous_db = tripal_db_set_active('chado');
		$xml_output = simplexml_load_string(db_result(db_query($sql, $analysis_id, $featureSet[$i])));
		
		$iteration = '';
		// new XML file parser has added the feature name within <Iteration_query-def> tags.
		if ($xml_output->getName() == 'Iteration') {
			$query = "";
			foreach ($xml_output->children() as $xml_tag) {
				if ($xml_tag->getName() == 'Iteration_query-def') {
					// Here we show the feature name again to check if we pull the correct data
					$query = $xml_tag;
				} else if ($xml_tag->getName() == 'Iteration_hits') {
					$iteration = $xml_tag;
				}
			}
	   // This is for the file parsed by the old parser
		} else {
			$iteration = $xml_output;
		}

		$number_hits = 0;
		foreach($iteration->children() as $hits){
			$number_hits ++;
		}

		$query = explode(" ", $query) ;
		$query = $query [0];
		if ($number_hits == 0) {
			continue;
		} 

		// now run through the blast hits/hsps of this iteration
		// and generate the rows of the table

		foreach($iteration->children() as $hits){
			$hit_count++;
			foreach($hits->children() as $hit){
				$best_evalue = 0;
				$best_identity = 0;
				$best_len = 0;
				$element_name = $hit->getName();
				if($element_name == 'Hit_id'){
					// if parsing "name, acc, desc" from three tags (1/3)
					if ($is_genbank) {
						$hit_name = $hit;
					}
				} else if($element_name == 'Hit_def'){
					if($is_genbank){
						$description = $hit;
					} else {
						$accession = preg_replace($regex_hit_accession,"$1",$hit);
						$hit_name = preg_replace($regex_hit_id,"$1",$hit);
						$description = preg_replace($regex_hit_def,"$1",$hit);
					}
				} else if($element_name == 'Hit_accession'){
					// if parsing "name, acc, desc" from three tags (3/3)
					if ($is_genbank){
						$accession = $hit;
					}
					// now run through each HSP for this hit
				} else if($element_name == 'Hit_hsps'){
					foreach($hit->children() as $hsp){
						foreach($hsp->children() as $hsp_info){
							$element_name = $hsp_info->getName();
							if($element_name == 'Hsp_num'){
								$hsp_num = $hsp_info;
							}
							if($element_name == 'Hsp_bit-score'){
								$hsp_bit_score = $hsp_info;
							}
							if($element_name == 'Hsp_score'){
								$hsp_score = $hsp_info;
							}
							if($element_name == 'Hsp_evalue'){
								$hsp_evalue = $hsp_info;
								// use the first evalue for this set of HSPs
								// as the best evalue. This get's shown as
								// info for the overall match.
								if(!$best_evalue){
									$best_evalue = $hsp_evalue;
								}
							}
							if($element_name == 'Hsp_query-from'){
								$hsp_query_from = $hsp_info;
							}
							if($element_name == 'Hsp_query-to'){
								$hsp_query_to = $hsp_info;
							}
							if($element_name == 'Hsp_hit-from'){
								$hsp_hit_from = $hsp_info;
							}
							if($element_name == 'Hsp_hit-to'){
								$hsp_hit_to = $hsp_info;
							}
							if($element_name == 'Hsp_query-frame'){
								$hsp_query_frame = $hsp_info;
							}
							if($element_name == 'Hsp_identity'){
								$hsp_identity = $hsp_info;
								// use the first evalue for this set of HSPs
								// as the best evalue. This get's shown as
								// info for the overall match.
								if(!$best_identity){
									$best_identity = $hsp_identity;
								}
							}
							if($element_name == 'Hsp_positive'){
								$hsp_positive = $hsp_info;
							}
							if($element_name == 'Hsp_align-len'){
								$hsp_align_len = $hsp_info;
								// use the first evalue for this set of HSPs
								// as the best evalue. This get's shown as
								// info for the overall match.
								if(!$best_len){
									$best_len = $hsp_align_len;
								}
							}
							if($element_name == 'Hsp_qseq'){
								$hsp_qseq = $hsp_info;
							}
							if($element_name == 'Hsp_hseq'){
								$hsp_hseq = $hsp_info;
							}
							if($element_name == 'Hsp_midline'){
								$hsp_midline = $hsp_info;
							}
						}
					}
				}
			}
			
			// Get analysisfeature_id
			$sql = "SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = %d AND feature_id = %d";
			$af_id = db_result(db_query($sql, $analysis_id, $featureSet[$i]));
			
			// Get type_id
			$sql = "SELECT cvterm_id FROM {cvterm} WHERE name = '%s' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'tripal')"; 
			$type_id = db_result(db_query($sql, 'analysis_blast_besthit_query'));
			
			$sql_test ="SELECT analysisfeatureprop_id FROM {analysisfeatureprop} WHERE analysisfeature_id = $af_id AND type_id = %d";
			$test_afpid = db_result(db_query($sql_test, $type_id));
			
			//Insert only if this blast query not exists. 
			if (!$test_afpid) {
				$afp_sql = "INSERT INTO {analysisfeatureprop} (analysisfeature_id, type_id, value, rank) VALUES (%d, %d, '%s', 0)";
				
				//$query;
				db_query($afp_sql, $af_id, $type_id, $query);

				//$hit_name;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_match'));
				db_query($afp_sql, $af_id, $type_id, $hit_name);
					
				//$description;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_description'));
				db_query($afp_sql, $af_id, $type_id, $description);
					
				//$best_evalue;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_evalue'));
				$e_digit = explode("e-", $best_evalue);
				if (count($e_digit) == 2) {
					$evalue_shown = number_format($e_digit [0],1);
					$best_evalue = $evalue_shown."e-".$e_digit[1];
				} 
				db_query($afp_sql, $af_id, $type_id, $best_evalue);
					
				//$best_identity;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_identity'));
				$percent_identity = number_format($best_identity/$best_len*100, 1);
				db_query($afp_sql, $af_id, $type_id, $percent_identity);
					
				//$best_len;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_length'));
				db_query($afp_sql, $af_id, $type_id, $best_len);
				
			// Otherwise, update all instead
			} else {
				$afp_sql = "UPDATE {analysisfeatureprop} SET analysisfeature_id = %d, type_id = %d, value = '%s', rank = 0 WHERE analysisfeatureprop_id = %d";

				//$query;
				db_query($afp_sql, $af_id, $type_id, $query, $test_afpid);
				
				//$hit_name;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_match'));
				$test_afpid = db_result(db_query($sql_test, $type_id));
				db_query($afp_sql, $af_id, $type_id, $hit_name, $test_afpid);

				//$description;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_description'));
				$test_afpid = db_result(db_query($sql_test, $type_id));
				db_query($afp_sql, $af_id, $type_id, $description, $test_afpid);
					
				//$best_evalue;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_evalue'));
				$test_afpid = db_result(db_query($sql_test, $type_id));
				$e_digit = explode("e-", $best_evalue);
				if (count($e_digit) == 2) {
					$evalue_shown = number_format($e_digit [0],1);
					$best_evalue = $evalue_shown."e-".$e_digit[1];
				} 
				db_query($afp_sql, $af_id, $type_id, $best_evalue, $test_afpid);
					
				//$best_identity;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_identity'));
				$test_afpid = db_result(db_query($sql_test, $type_id));
				$percent_identity = number_format($best_identity/$best_len*100, 1);
				db_query($afp_sql, $af_id, $type_id, $percent_identity, $test_afpid);
					
				//$best_len;
				$type_id = db_result(db_query($sql, 'analysis_blast_besthit_length'));
				$test_afpid = db_result(db_query($sql_test, $type_id));
				db_query($afp_sql, $af_id, $type_id, $best_len, $test_afpid);
				
			}
			tripal_db_set_active($previous_db);
			
			break;
		}
	}
   print "100%\n";
	return;
}
