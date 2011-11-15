<?php

/**
 * Form to initialize an align library tripal job
 */
function tripal_gbrowse_align_features_form ($form_state) {
  $form = array();
  
  // compile all analysis' as options
  $analysis_options = array();
  $results = tripal_core_chado_select('analysis',array('analysis_id','name', 'program'), array());
  foreach ($results as $r) {
    $analysis_options[ 'A'.$r->analysis_id ] = $r->name .' --'.$r->program;
  }
  
  // compile all libraries as options
  $library_options = array();
  $results = tripal_core_chado_select('library',array('library_id','name', 'uniquename'), array());
  foreach ($results as $r) {
    $library_options[ 'L'.$r->library_id ] = $r->name .' --'.$r->uniquename;
  }  
  
  $form['description'] = array(
    '#type' => 'item',
    '#value' => 'This form allows you to select a query library/analysis and align the '
      .'features that are part of that query library/analysis against those from a database '
      .'library/analysis using BLAST. Depending upon the Alignment Criteria, the top BLAST '
      .'result(s) for each query feature will be used to determine the location of the query '
      .'feature on a database feature.',
  );
  
  $form['q'] = array(
    '#type' => 'fieldset',
    '#title' => 'Features to be Aligned (Query)',
    '#description' => 'Please select the library or analysis below which groups the features you '
      .'want aligned together.',
  );
  
  $form['q']['query'] = array(
    '#type' => 'select',
    '#title' => 'Query Features',
    '#options' => array(
      'Libraries' => $library_options,
      "Analaysis'" => $analysis_options
    ),
    '#default_value' => $form_state['values']['query'],
  );
  
  $form['d'] = array(
    '#type' => 'fieldset',
    '#title' => 'Features to Align To (Database)',
    '#description' => 'Please select the library of analysis below, which contains the features '
      .'you want to align the query features selected above to. Only features in the selected '
      .'library/analysis with sequence data will be used.',
  );

  $form['d']['database'] = array(
    '#type' => 'select',
    '#title' => 'Database Features',
    '#options' => array(
      'Libraries' => $library_options,
      "Analaysis'" => $analysis_options
    ),
    '#default_value' => $form_state['values']['database'],
  );
  
  $form['b'] = array(
    '#type' => 'fieldset',
    '#title' => 'Alignment Criteria',
  );
  
  $form['b']['program'] = array(
  	'#type' => 'radios',
  	'#title' => 'Alignment Program',
  	'#options' => array(
  		'blast' => 'BLAST',
  		'blat' => 'BLAT',
  	),
  	'#default_value' => 'blat',
  );

  $form['b']['blat'] = array(
  	'#type' => 'fieldset',
  	'#title' => 'BLAT Options',
  	'#collapsible' => TRUE,
		'#collapsed' => TRUE,
  );
  
  $form['b']['blat']['min_identity'] = array(
  	'#type' => 'textfield',
  	'#title' => 'Minimum Identity',
  	'#default_value' => '90',
  );
  
  $form['b']['blast'] = array(
  	'#type' => 'fieldset',
  	'#title' => 'BLAST Options',
  	'#collapsible' => TRUE,
		'#collapsed' => TRUE,
  );
  
  $form['b']['blast']['gapped_alignment'] = array(
  	'#type' => 'checkbox',
  	'#title' => 'Gapped Alignment?',
  	'#default_value' => TRUE,
  );
  
  $form['b']['blast']['evalue'] = array(
  	'#type' => 'textfield',
  	'#title' => 'Expectation Values (E-value) Cutoff',
  	'#description' => 'To enter in scientific notation (ie: 5 x 10-6), enter [base number]e[power] (ie: 5e-6)',
  	'#default_value' => 10.0
  );
  
  $form['g'] = array(
  	'#type' => 'fieldset',
  	'#title' => 'GBrowse Information',
  );
  
  $form['g']['source'] = array(
  	'#type' => 'textfield',
  	'#title' => 'Source of Features',
  	'#description' => 'The source of the features grouped by the selected query library'
  );
  
  //Sending query to the database
  $resource = db_query('SELECT * FROM {tripal_gbrowse_instances}');
	$items = array();
	while($record = db_fetch_object($resource)){
		$items[$record->gbrowse_id]= $record->gbrowse_name;
	}
   
  //GBrowse Instances
  $form['g']['gbrowse_id'] = array(
    '#type' => 'select',
    '#title' => t('GBrowse Instances'),
    '#options' => $items,
    '#description' => t('Selected GBrowse Instances to load the query features into.'),
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#name' => 'align',
    '#value' => 'Align Features',
  );
  
  return $form;
}

/**
 * Form submit to initialize an align library tripal job
 */
function tripal_gbrowse_align_features_form_submit ($form, &$form_state) {
  
  switch($form_state['clicked_button']['#name']) {
    case 'align':
      $options = array();
      // Query
      if (preg_match('/(L|A)(\d+)/',$form_state['values']['query'], $matches)) {
        if ($matches[1] == 'L') {
          $options['query_type'] = 'library';
        } else {
          $options['query_type'] = 'analysis';
        }
        $options['query_id'] = $matches[2];
      }
      
      // Database
      if (preg_match('/(L|A)(\d+)/',$form_state['values']['database'], $matches)) {
        if ($matches[1] == 'L') {
          $options['db_type'] = 'library';
        } else {
          $options['db_type'] = 'analysis';
        }
        $options['db_id'] = $matches[2];
      } 
      
      $options['source'] = $form_state['values']['source'];
      
      // db options
      $sql = 'SELECT * FROM {tripal_gbrowse_instances} WHERE gbrowse_id = %d';
      $r = db_fetch_object(db_query($sql,$form_state['values']['gbrowse_id']));
      $options['dbname'] = $r->database_name;
      $options['dbuser'] = $r->database_user;
      $options['dbpass'] = $r->user_password;

			if ($form_state['values']['program'] == 'blat') { 
				// blat options
				$options['min_identity'] = $form_state['values']['min_identity'];
				
				global $user;
				tripal_add_job(
					'Align '.$options['query_type'].' features ('.$options['query_type'].'_id='.$options['query_id'].')',
					'tripal_gbrowse',
					'tripal_gbrowse_align_features_by_blat',
					array(serialize($options)),
					$user->uid
				);			
			} elseif ($form_state['values']['program'] == 'blast') {      
				// blast options
				$options['evalue'] = $form_state['values']['evalue'];
				$options['gapped_alignment'] = $form_state['values']['gapped_alignment'];
			
				global $user;
				tripal_add_job(
					'Align '.$options['query_type'].' features ('.$options['query_type'].'_id='.$options['query_id'].')',
					'tripal_gbrowse',
					'tripal_gbrowse_align_features_by_blast',
					array(serialize($options)),
					$user->uid
				);
      }
      
    break;
  }
}

/**
 * Aligns feature from a query library/analysis to a database library/analysis,
 * saving the results as a GFF3 file and then loading it into the selected gbrowse instance
 *
 * @param $options
 *   An array containing options needed to align features and create featurelocs
 *    -query_type: the type of chado grouping containing query features (either library or analysis)
 *    -query_id: the library_id/analysis_id containing the query features
 *    -db_type: the type of chado grouping containing database features (either library or analysis)
 *    -db_id: the library_id/analysis_id containing the database features
 *    -dbname: name of the MySQL GBrowse database to load into
 *    -dbuser: name of the user with permission to load into the above database
 *    -dbpass: the password for the above user
 */
function tripal_gbrowse_align_features_by_blast ($options) {
  $options = unserialize($options);
  print 'Query: '.$options['query_type'].' where '.$options['query_type'].'_id='.$options['query_id']."\n";
  print 'Database: '.$options['db_type'].' where '.$options['db_type'].'_id='.$options['db_id']."\n";
  
  // Generate FASTA ---------------------------------------
  print "\nGenerating fasta files for query and database...\n";
  $query_file = tripal_gbrowse_export_fasta_for_features( $options['query_type'], $options['query_id'], TRUE );
  print "\t".$query_file['file']."\n";
  
  $db_file = tripal_gbrowse_export_fasta_for_features( $options['db_type'], $options['db_id'], TRUE, TRUE );
  print "\t".$db_file['file']."\n";
  
  // Align using BLAST ------------------------------------
  print "\nAligning features using BLAST...\n";
  
  print "\tFormating Database FASTA into BLASTdb...\n";
  $db = '/tmp/exported_'.$options['db_type'].'_'.$options['db_id'];
  $formatdb_cmd = 'formatdb -n '.$db.' -p F -i '.$db_file['file'];
	print "\t\t".$formatdb_cmd."\n";  
  exec($formatdb_cmd);
	
	print "\tExecuting BLAST...\n";
	$blast_outfile = $db.'_by_'.$options['query_type'].'_'.$options['query_id'].'.blast.xml';
	$blastall_cmd = 'blastall -p blastn -d '.$db.' -i '.$query_file['file'].' -m 7 -o '.$blast_outfile.' -e '.$options['evalue'];
	if ($options['gapped_alignment']) {
		$blastall_cmd .= ' -g';
	}
	print "\t\t".$blastall_cmd."\n";
	exec($blastall_cmd);
  
  // Parse BLAST results ----------------------------------
  print "\nParsing Blast Results into GFF3...\n";
  $gff3_file = $db.'_by_'.$options['query_type'].'_'.$options['query_id'].'.gff3';
  $fgff3 = fopen($gff3_file, 'w');
  fwrite($fgff3, "##gff-version 3\n");
  $iteration = tripal_gbrowse_get_next_xml_record ($blast_outfile, "<Iteration>");
  while ($iteration) {
  	//print "Record:".$iteration['record']->asXML()."\n";
  	//print "Query: ".$iteration['record']->{'Iteration_query-def'}."\n";
		
		// Find the best Hit by looking at the bit scores of the hsps
		// the larger the bit score the better the alignment
		$hits = array();
		$scores = array();
  	foreach ($iteration['record']->Iteration_hits->Hit as $hit) {
  		$score = 0;
  		$num = 0;
  		foreach ($hit->{'Hit_hsps'}->{'Hsp'} as $hsp) {
  			//print 'HSP:'.$hsp->asXML()."\n";
  			$score = $score + $hsp->{'Hsp_bit-score'};
  			$num++;
  		}
  		$avg = round($score / $num,2);
  		$hit->{'Hit_bit-score'} = $avg;
  		$hits[] = array('score' => $avg, 'hit' => $hit);
  	}
  	usort($hits, 'tripal_gbrowse_sort_by_score');
  	$best_hit = $hits[0]['hit'];
  	//print "\tBest Hit:".$best_hit->Hit_def.' ('.$best_hit->{'Hit_bit-score'}.")\n";
  	
  	// generate gff3 for the best hit 
  	$query_name = $iteration['record']->{'Iteration_query-def'};
  	$db_name = $best_hit->Hit_def;  
  	if (isset($db_file['noseq_features'][$db_name])) {
  	  $db_offset = $db_file['noseq_features'][$db_name]['start'];
  	  $db_name = $db_file['noseq_features'][$db_name]['parent']['uniquename'];
  	} else {
  	  $db_offset = 0;
  	}
		$lines = array();
		$hit_start = 99999999999999999999;
		$hit_end = 0;
  	foreach ($best_hit->{'Hit_hsps'}->{'Hsp'} as $hsp) {
  		if ($hit_start > (int) $hsp->{'Hsp_hit-from'}[0]) { $hit_start = (int) $hsp->{'Hsp_hit-from'}[0]; } 
  		if ($hit_end < (int) $hsp->{'Hsp_hit-to'}[0]) { $hit_end = (int) $hsp->{'Hsp_hit-to'}; }
  		$lines[] = implode("\t", array(
  			$db_name, 
  			$options['source'],
  			'match_part',
  			$hsp->{'Hsp_hit-from'} + $db_offset,
  			$hsp->{'Hsp_hit-to'} + $db_offset,
  			$hsp->{'Hsp_bit-score'},
  			'.',
  			'.',
  			'ID='.$query_name.'_'.$hsp->{'Hsp_num'}.';Parent='.$query_name
  		))."\n";
  	}
  	fwrite($fgff3, implode("\t", array(
			$db_name, 
			$options['source'],
			'match',
			$hit_start + $db_offset,
			$hit_end + $db_offset,
			$best_hit->{'Hit_bit-score'},
			'.',
			'.',
			'ID='.$query_name.';Name='.$query_name
		))."\n");
		fwrite($fgff3,implode('',$lines));
  	
  	// get next iteration xml record
  	$last_iteration = $iteration;
  	//print "Getting Iteration...\n";
  	$iteration = tripal_gbrowse_get_next_xml_record ($blast_outfile, "<Iteration>", $last_iteration['start_line_num']);
  }
  
  print "\nLoading GFF3 into GBrowse...\n";
  //The loading script: bp_seqfeature_load.pl, allows loading of data to specific file 
  $command= "bp_seqfeature_load.pl -u '" .$options['dbuser']. "' -p '" .$options['dbpass']. "' -d " .$options['dbname']. " " . $gff3_file;
  print "\t".$command."\n";

  exec($command);
}

/**
 * Aligns feature from a query library/analysis to a database library/analysis,
 * saving the results as a GFF3 file and then loading it into the selected gbrowse instance
 *
 * @param $options
 *   An array containing options needed to align features and create featurelocs
 *    -query_type: the type of chado grouping containing query features (either library or analysis)
 *    -query_id: the library_id/analysis_id containing the query features
 *    -db_type: the type of chado grouping containing database features (either library or analysis)
 *    -db_id: the library_id/analysis_id containing the database features
 *    -dbname: name of the MySQL GBrowse database to load into
 *    -dbuser: name of the user with permission to load into the above database
 *    -dbpass: the password for the above user
 */
function tripal_gbrowse_align_features_by_blat ($options,$job_id) {
  $options = unserialize($options);
  print 'Query: '.$options['query_type'].' where '.$options['query_type'].'_id='.$options['query_id']."\n";
  print 'Database: '.$options['db_type'].' where '.$options['db_type'].'_id='.$options['db_id']."\n";
  
  // Generate FASTA ---------------------------------------
  print "\nGenerating fasta files for query and database...\n";
  $query_file = tripal_gbrowse_export_fasta_for_features( $options['query_type'], $options['query_id'], TRUE );
  print "\t".$query_file['file']."\n";
  tripal_job_set_progress($job_id, 12);
  
  $db_file = tripal_gbrowse_export_fasta_for_features( $options['db_type'], $options['db_id'], TRUE, TRUE );
  print "\t".$db_file['file']."\n";
  tripal_job_set_progress($job_id, 25);
  
  // Align using BLAT ------------------------------------
  print "\nAligning features using BLAT...\n";
	$blat_outfile = '/tmp/alignment_'.$options['db_type'].'-'.$options['db_id'].'_by_'.$options['query_type'].'-'.$options['query_id'].'.psl';
	$blat_cmd = 'blat '.$db_file['file'].' '.$query_file['file'].' -q=dnax -t=dnax -noHead -minIdentity='.$options['min_identity'].' '.$blat_outfile;
	print "\t\t".$blat_cmd."\n";
	exec($blat_cmd);
	tripal_job_set_progress($job_id, 50);
	
	// Parse BLAST results ----------------------------------
	$total_lines = trim(`wc --lines < $blat_outfile`);
	$interval = intval($total_lines/5);
	$percent_per_line = 25/$total_lines;
	$num_lines = 0;
	$query_seq = array();
	
	print "\nParsing BLAT results into GFF3 (".$total_lines." lines)...\n";
	$gff3_file = '/tmp/alignment_'.$options['db_type'].'-'.$options['db_id'].'_by_'.$options['query_type'].'-'.$options['query_id'].'.gff3';
  $fgff3 = fopen($gff3_file, 'w');
  fwrite($fgff3, "##gff-version 3\n");
	$bh = fopen($blat_outfile, 'r');
	while (!feof($bh)) {
		$line = explode("\t", fgets($bh));
		$num_lines++;
		if (($num_lines%$interval) == 0) { tripal_job_set_progress($job_id, intval($percent_per_line * $num_lines)); }
		
		$print_match = TRUE;
    $db_name = $line[13];
  	if (isset($db_file['noseq_features'][$db_name])) {
  	  $db_offset = $db_file['noseq_features'][$db_name]['start'];
  	  $db_name = $db_file['noseq_features'][$db_name]['parent']['uniquename'];
  	} else {
  	  $db_offset = 0;
  	}
  	
  	$query_id = $line[9];
  	if (!isset($query_seq[ $line[9] ])) {
  		$query_seq[ $line[9] ]['id'] = 0;
  		$query_seq[ $line[9] ]['start'] = $line[15] + $db_offset;
  		$query_seq[ $line[9] ]['end'] = $line[16] + $db_offset;
  		$query_id .= '_0';
  	} elseif ( abs($line[15]+$db_offset-$query_seq[ $line[9] ]['start']) < 5 ) {
  		$print_match = FALSE;
  		$query_id .= '_' . $query_seq[ $line[9] ]['id'];
  	}else {
  		$query_seq[ $line[9] ]['id']++;
  		$query_id .= '_' . $query_seq[ $line[9] ]['id'];
  	}
  	
		// match line
		if ($print_match) {
			fwrite($fgff3, implode("\t", array(
				$db_name, 
				$options['source'],
				'match',
				$line[15] + $db_offset,
				$line[16] + $db_offset,
				'.',
				$line[8][1],
				'.',
				'ID='.$query_id.';Name='.$line[9]
			))."\n");
		}
		
		// match parts
		$parts_size = explode(',',trim($line[18]));
		$parts_start = explode(',',trim($line[20]));
		foreach ($parts_size as $k => $length) {

			if (!empty($parts_start[$k])) {
				fwrite($fgff3, implode("\t", array(
					$db_name, 
					$options['source'],
					'match_part',
					$parts_start[$k] + $db_offset,
					$parts_start[$k] + $length + $db_offset,
					'.',
					$line[8][1],
					'.',
					'ID='.$query_id.'_'.$k.';Parent='.$query_id
				))."\n");
  		}
  		
		}
	}
	tripal_job_set_progress($job_id, 75);
	
	// Load into GBrowse ------------------------------------
  print "\nLoading GFF3 into GBrowse...\n";
  //The loading script: bp_seqfeature_load.pl, allows loading of data to specific file 
  $command= "bp_seqfeature_load.pl -u '" .$options['dbuser']. "' -p '" .$options['dbpass']. "' -d " .$options['dbname']. " " . $gff3_file;
  print "\t".$command."\n";
	exec($command);

}

/**
 * Creates a fasta file for a given chado grouping of features
 *
 * @param $type
 *   The type of chado grouping. Allowed values are either library or analysis
 * @param $id
 *   The library_id/analysis_id of the chado grouping
 * @return
 *   The name of the multi-fasta file containing records for all features with residues
 *   in the supplied library/analysis
 */
function tripal_gbrowse_export_fasta_for_features ($type, $id, $use_parent_seq = FALSE, $save_offset = FALSE) {

  $fasta_file = '/tmp/exported_fasta-'.$type.'-'.$id.'.fasta';
  $noseq_features = array();
  
  $fh = fopen($fasta_file,'w');
  
  $sql = 'SELECT f.uniquename, f.residues, fl.fmin, fl.fmax, fl.srcfeature_id as parent_feature_id FROM feature f '
  	.'LEFT JOIN featureloc fl ON fl.feature_id=f.feature_id ';
  $parent_sql = 'SELECT p.feature_id as parent_feature_id, p.uniquename as parent_uniquename, count(fl.feature_id) FROM feature p '
  	.'LEFT JOIN featureloc fl ON fl.srcfeature_id=p.feature_id '
  	."WHERE fl.feature_id IN (SELECT feature_id FROM feature WHERE residues='') AND ";
  if ($type == 'library') { 
  	$sql .= 'LEFT JOIN library_feature lf ON f.feature_id=lf.feature_id WHERE lf.library_id=%d';
  	$parent_sql .= 'fl.feature_id IN (SELECT feature_id FROM library_feature WHERE library_id=%d) GROUP BY p.uniquename, p.feature_id';
    
  } elseif ($type == 'analysis') {
  	$sql .= 'LEFT JOIN analysisfeature af ON f.feature_id=af.feature_id WHERE af.analysis_id=%d';
		$parent_sql .= 'fl.feature_id IN (SELECT feature_id FROM analysisfeature WHERE analysis_id=%d) GROUP BY p.uniquename, p.feature_id';
    $resource = db_query($sql, $id);
  } else {
    print "ERROR: Unable to generate FASTA due to unrecognized type -".$type."!\n";
    return FALSE;
  }

  
  // check if some don't have sequence but are aligned on a parent who does
	//print "SQL: ".$parent_sql."\n";
  $resource = db_query($parent_sql, $id);
	$parent_seq = array();
	$residues_seq = 'SELECT residues FROM feature WHERE feature_id=%d';
  while ($r = db_fetch_object($resource)) {
  	//print 'Creating index for '.$r->parent_feature_id. " (".$r->parent_uniquename.")\n";
  	$seq = db_fetch_object(db_query($residues_seq, $r->parent_feature_id));
  	//print "Residues:".$seq->residues."\n";
  	if (!empty($seq->residues)) {
	  	//print "\tGot Seq!\n";
	  }
		$parent_seq[ $r->parent_feature_id ] = array(
			'residues' => $seq->residues,
			'uniquename' => $r->parent_uniquename
		);
  }
  
	$resource = db_query($sql, $id);
  while ($r = db_fetch_object($resource)) {
    if (!empty($r->residues)) {
      fwrite($fh, '>'.$r->uniquename."\n");
      fwrite($fh, wordwrap($r->residues,80,"\n",TRUE)."\n");
    } else {
	    //print $r->uniquename." (based on ".$r->parent_feature_id.' -'.$parent_seq[$r->parent_feature_id]['uniquename'].")\n";
      if (!empty($parent_seq[$r->parent_feature_id]['residues'])) {
      	//print "\tHave Seq\n";
      	fwrite($fh, '>'.$r->uniquename." (based on ".$parent_seq[$r->parent_feature_id]['uniquename'].")\n");
        $seq = substr($parent_seq[$r->parent_feature_id]['residues'], $r->fmin, $r->fmax - $r->fmin);
        fwrite($fh, wordwrap($seq,80,"\n",TRUE)."\n");
        if ($save_offset) {
          $noseq_features[ $r->uniquename ] = array(
            'start' => $r->fmin,
            'end' => $r->fmax,
            'parent' => array(
              'uniquename' => $parent_seq[$r->parent_feature_id]['uniquename'],
            ),
          );
        }
      }
    }
  }
  
  fclose($fh);
  return array(
    'file' => $fasta_file,
    'noseq_features' => $noseq_features
  );
}

/**
 * Retrieves the next xml record with $record_identifier
 *
 * Assumption: the end tag for $record_identifier is one line before the next opening tag
 * @param $xml_file
 *   The file containing the xml records; must supply the full path
 * @param $record_identifier
 *   The opening tag enclosing a record (ie: <Iteration>)
 * @param $last_record_line_num
 *   The line number of the openin tag for the last record
 * 
 * @return
 *   An array describing the next xml record
 *    -record: the simpleXML record
 *    -start_line_num: the line number in the file that this record starts at
 *    -end_line_num: the line number in the file that this record ends at
 */
function tripal_gbrowse_get_next_xml_record ($xml_file, $record_identifier, $last_record_line_num = NULL) {

	// If first record ------------------------------------------------
	if (!$last_record_line_num) {
		$cmd = 'grep -n "'.$record_identifier.'" -m 2 '.$xml_file;
		exec($cmd, $line_num_raw);

		// get start of next record
		if (preg_match('/(\d+):.*/',$line_num_raw[0],$matches)) {
			$start = $matches[1];
		} else {
			return FALSE;
		}

		// get end of next record
		if (preg_match('/(\d+):.*/',$line_num_raw[1],$matches)) {
			$end = $matches[1] -1;
		} else {
			return FALSE;
		}

	// If not first record --------------------------------------------
	} else {

		$cmd = 'tail --lines=+'.($last_record_line_num+1).' '.$xml_file.' 2>/dev/null | grep -n "'.$record_identifier.'" -m 2 2>/dev/null ';
		exec($cmd, $line_num_raw);

		// get start of next record
		if (preg_match('/(\d+):.*/',$line_num_raw[0],$matches)) {
			$start = $matches[1] + $last_record_line_num;
		} else {
			return FALSE;
		}

		// get end of next record
		if (preg_match('/(\d+):.*/',$line_num_raw[1],$matches)) {
			$end = $matches[1] + $last_record_line_num -1;
		} else {
			return FALSE;
		}
		
	}
	
	$cmd = 'tail --lines=+'.$start.' '.$xml_file.' 2>/dev/null | head -n '.($end-$start+1).' 2>/dev/null';
	exec($cmd, $xml);
	
	//print "XML:".implode("\n",$xml)."\n";
	if (!$xml) {
		return FALSE;
	}
	
	$xml_record = new SimpleXMLElement(implode("\n",$xml));
	return array(
		'record' => $xml_record,
		'start_line_num' => $start,
		'end_line_num' => $end,
	);
}

/**
 * Custom sort function to be used with usort
 * Sorts an array( 'score' => \d+, 'hit' => simplexml obj)
 */
function tripal_gbrowse_sort_by_score ($a, $b) {

	if ($a['score'] == $b['score']) {
		return 0;
	} elseif ($a['score'] < $b['score']) {
		return 1;
	} else {
		return -1;
	}
}