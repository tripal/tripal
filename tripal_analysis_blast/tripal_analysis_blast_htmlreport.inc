<?php

function tripal_get_blast_report ($analysis_id, $currentpage, $sort, $descending, $per_page) {
	if (!tripal_analysis_check_permission($analysis_id)) {
		$html_out = "Access denied. You are not authorized to access this report.";
		return $html_out;
	}
	$report_object = new stdClass;
	
	$symbol = "&and;";
	$sort_key = "ASC";
	if ($descending == 1) {
		$symbol = "&or;";
		$sort_key = "DESC";
	}
	$header = 'analysis_blast_besthit_query';
	$convert = 0;
	if ($sort == 1) {
		$header = 'analysis_blast_besthit_match';
	} else if ($sort == 2) {
		$header = 'analysis_blast_besthit_description';
	} else if ($sort == 3) {
		$header = 'analysis_blast_besthit_evalue';
		$convert = 1;
	} else if ($sort == 4) {
		$header = 'analysis_blast_besthit_identity';
		$convert = 1;
	} else if ($sort == 5) {
		$header = 'analysis_blast_besthit_length';
		$convert = 1;
	}

	$previous_db = tripal_db_set_active('chado');
	// Get analysis information including 'Time', 'Name', and 'DB Settings'
	$sql = "SELECT value, name, to_char(timeexecuted, 'MM-DD-YYYY') AS time
                FROM {analysis}  A
                INNER JOIN {analysisprop} AP ON  A.analysis_id = AP.analysis_id 
                WHERE A.analysis_id = %d
                AND type_id= (SELECT cvterm_id 
                                           FROM {cvterm} 
                                           WHERE name = 'analysis_blast_blastdb')";
	$analysis = db_fetch_object(db_query($sql, $analysis_id));
	$db_id = $analysis->value;
	
	// Get db 'urlprefix'
	$sql = "SELECT urlprefix FROM {db} WHERE db_id=%d";
	$urlprefix = db_result(db_query($sql, $db_id));
	
	// Get all analysisfeature_id's sorted by the selected header for this blast analysis
	$sql = "SELECT AFP.analysisfeature_id
	             FROM {analysisfeature} AF 
	             INNER JOIN {analysisfeatureprop} AFP ON AF.analysisfeature_id = AFP.analysisfeature_id
	             WHERE analysis_id = %d
	             AND AFP.type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = '%s' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'tripal'))
	             ORDER BY value ";
	// Convert string to number for evalue, %identity, and length before SQL sorting
	if ($convert == 1) {
		$sql .= "::numeric  ";
	} 
	$sql .= "$sort_key";

	$result = db_query($sql, $analysis_id, $header);
	tripal_db_set_active($previous_db);
	$analysisfeatureSet = array();
	// Count how many analysisfeature_id we have and store them in order in $analysisfeatureSet
	$counter = 0;	
	while ($feature = db_fetch_object($result)) {
		$analysisfeatureSet [$counter] = $feature->analysisfeature_id;
		$counter ++;
	}
   
	// Get analysis node id
	$ana_nid = db_result(db_query("SELECT nid FROM {chado_analysis} WHERE analysis_id = %d", $analysis_id));
	$ana_url = url("node/".$ana_nid);
	
	// Get pager info
	$no_page = (int) ($counter / $per_page);
	if ($counter % $per_page != 0) {
		$no_page ++;
	}
	$first_item = ($currentpage - 1)* $per_page;

	$report_object->time = $analysis->time;
	$report_object->name = $analysis->name;
	$report_object->url = $ana_url;
	$report_object->counter = $counter;
	$report_object->currentpage = $currentpage;
	$report_object->no_page = $no_page;
	$report_object->symbol = $symbol;
	$report_object->sort = $sort;

	// Construct URL path for different sorting column
	$path =  "tripal_blast_report/$analysis_id/1/0/";
	if ($descending == 0 && $sort == 0) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$report_object->byQuery = $url_path;

	$path =  "tripal_blast_report/$analysis_id/1/1/";
	if ($descending == 0 && $sort == 1) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$report_object->byMatchName =$url_path;

	$path =  "tripal_blast_report/$analysis_id/1/2/";
	if ($descending == 0 && $sort == 2) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$report_object->byDescription = $url_path;

	$path =  "tripal_blast_report/$analysis_id/1/3/";
	if ($descending == 0 && $sort == 3) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$report_object->byEvalue = $url_path;
	
	$path =  "tripal_blast_report/$analysis_id/1/4/";
	if ($descending == 0 && $sort == 4) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$report_object->byIdentity = $url_path;

	$path =  "tripal_blast_report/$analysis_id/1/5/";
	if ($descending == 0 && $sort == 5) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$report_object->byLength = $url_path;

	// Get table content
	$sql = "SELECT value
	             FROM {analysisfeatureprop} 
	             WHERE analysisfeature_id = %d 
	             AND type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = '%s' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'tripal'))";
	$j = 0;
	$hits = array ();
	for ($i = $first_item; $i < $first_item +$per_page; $i ++) {
		$previous_db = tripal_db_set_active('chado');
		$query = db_result(db_query($sql, $analysisfeatureSet[$i], 'analysis_blast_besthit_query'));
		$match = db_result(db_query($sql, $analysisfeatureSet[$i], 'analysis_blast_besthit_match'));
		$desc = db_result(db_query($sql, $analysisfeatureSet[$i], 'analysis_blast_besthit_description'));
		$evalue = db_result(db_query($sql, $analysisfeatureSet[$i], 'analysis_blast_besthit_evalue'));
		$identity = db_result(db_query($sql, $analysisfeatureSet[$i], 'analysis_blast_besthit_identity'));
		$length = db_result(db_query($sql, $analysisfeatureSet[$i], 'analysis_blast_besthit_length'));
		
		$sql_fid = "SELECT feature_id FROM analysisfeature WHERE analysisfeature_id = %d";
		$fid = db_result(db_query($sql_fid, $analysisfeatureSet[$i]));
		tripal_db_set_active($previous_db);
		
		$q_url = url("ID$fid");
		$class = 'tripal_analysis_blast-table-odd-row tripal-table-odd-row';
	 	if($j % 2 == 0 ){
         $class = 'tripal_analysis_blast-table-even-row tripal-table-even-row';
      }
      $hit = new stdClass();
		if ($query) {
			$hit->class = $class;
			$hit->q_url = $q_url;
			$hit->query = $query;
			$hit->urlprefix = $urlprefix;
			$hit->match = $match;
			$hit->desc = $desc;
			$hit->evalue = $evalue;
			$hit->identity = $identity;
			$hit->length = $length;
			$hits [$j] = $hit;
		}
		$j ++;
	}
	$report_object->hits = $hits;
	
	// The number of items to show on the page
	$path =  "tripal_blast_report/$analysis_id/1/$sort/$descending/";
	$report_object->path = $path;
	$report_object->per_page = $per_page;

	// Make AJAX call to update the  page which user selected 
	$report_object->analysis_id = $analysis_id;
	$report_object->sort = $sort;
	$report_object->descending = $descending;
	$report_object->per_page = $per_page;
	$report_object->no_page = $no_page;

    return theme('tripal_analysis_blast_report',$report_object);
}