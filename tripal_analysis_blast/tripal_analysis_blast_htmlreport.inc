<?php

function tripal_get_blast_report ($analysis_id, $currentpage, $sort, $descending, $per_page) {
	/*
	if (!tripal_check_permission_by_analysis_id($analysis_id)) {
		$html_out = "Access denied. You are not authorized to access this report.";
		return $html_out;
	}
   */
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
                                           WHERE name = 'analysis_blast_settings')";
	$analysis = db_fetch_object(db_query($sql, $analysis_id));
	
	// Parse the blast settings
	$blastsettings = explode("|", $analysis->value);
	$db_id = $blastsettings [0];
	
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
	$url = url(drupal_get_path('theme', 'tripal')."/images/ajax-loader.gif");
	$html_out .= "<div id=\"tripal_ajaxLoading\" style=\"display:none\">".
                   "<div id=\"loadingText\">Loading...</div>".
                   "<img src=\"$url\"></div>";
	$html_out .= "<div id=\"blast-hits-report\">
                             <strong>Analysis Date: </strong>$analysis->time (<a href=$ana_url>$analysis->name</a>)<br>
                             There are <strong>$counter</strong> records. <div id=tripal_blast_report_current_page>page <strong>$currentpage</strong> of <strong>$no_page</strong></div>
	                          <table id=\"tripal_analysi_blast-report-table\" class=\"tripal_analysis_blast-table tripal-table tripal-table-horz\">
	                            <tr class=\"tripal_analysis_blast-table-odd-row tripal-table-odd-row\">";

	$html_out .= "  <th nowrap>";	if ($sort == 0) {$html_out .= $symbol;}

	// Construct URL path for different sorting column
	$path =  "tripal_blast_report/$analysis_id/1/0/";
	if ($descending == 0 && $sort == 0) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$html_out.="<a href=$url_path>Query</a></th><th nowrap>";	if ($sort == 1) {$html_out .= $symbol;}

	$path =  "tripal_blast_report/$analysis_id/1/1/";
	if ($descending == 0 && $sort == 1) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$html_out.="<a href=$url_path>Match Name</a></th><th nowrap>";	if ($sort == 2) {$html_out .= $symbol;}

	$path =  "tripal_blast_report/$analysis_id/1/2/";
	if ($descending == 0 && $sort == 2) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$html_out.="<a href=$url_path>Description</a></th><th nowrap>";	if ($sort == 3) {$html_out .= $symbol;}

	$path =  "tripal_blast_report/$analysis_id/1/3/";
	if ($descending == 0 && $sort == 3) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$html_out.="<a href=$url_path>E-value</a></th><th nowrap>";	if ($sort == 4) {$html_out .= $symbol;}

	$path =  "tripal_blast_report/$analysis_id/1/4/";
	if ($descending == 0 && $sort == 4) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$html_out.="<a href=$url_path>%Identity</a></th><th nowrap>";	if ($sort == 5) {$html_out .= $symbol;}

	$path =  "tripal_blast_report/$analysis_id/1/5/";
	if ($descending == 0 && $sort == 5) {
		$path .= "1";
	} else {
		$path .= "0";
	}
	$path .= "/$per_page";
	$url_path = url ($path);
	$html_out.="<a href=$url_path>Length</a></th>";

	$html_out .=      "</tr>";

	// Get table content
	$sql = "SELECT value
	             FROM {analysisfeatureprop} 
	             WHERE analysisfeature_id = %d 
	             AND type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = '%s' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'tripal'))";
	$j = 0;
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
		if ($query) {
			$html_out .= "<tr class=\"$class\">
		                           <td nowrap><a href=$q_url>$query</a></td>
		                           <td nowrap><a href=\"$urlprefix$match\" target=\"_blank\">$match</td>
		                           <td>$desc</td>
		                           <td nowrap>$evalue</td>
		                           <td nowrap>$identity</td>
		                           <td nowrap>$length</td>
		                         </tr>";
		}
		$j ++;
	}
	
	// The number of items to show on the page
	$path =  "tripal_blast_report/$analysis_id/1/$sort/$descending/";
	$html_out .= '</table>
	                           <div id=tripal_blast_report_per_page>Show ';
	if ($per_page == 10) {
		$html_out .= "<strong>10</strong> | ";
	} else {
		$url_path = url($path."10");
		$html_out .= "<a href=$url_path>10</a> | ";
	}
	if ($per_page == 20) {
		$html_out .= "<strong>20</strong> | ";
	} else {
		$url_path = url($path."20");
		$html_out .= "<a href=$url_path>20</a> | ";
	}
	if ($per_page == 50) {
		$html_out .= "<strong>50</strong> | ";
	} else {
		$url_path = url($path."50");
		$html_out .= "<a href=$url_path>50</a> | ";
	}
	if ($per_page == 100) {
		$html_out .= "<strong>100</strong> | ";
	} else {
		$url_path = url($path."100");
		$html_out .= "<a href=$url_path>100</a> | ";
	}
	if ($per_page == 200) {
		$html_out .= "<strong>200</strong> | ";
	} else {
		$url_path = url($path."200");
   	$html_out .= "<a href=$url_path>200</a>";
	}

	// Make AJAX call to update the  page which user selected 
   $html_out .= ' records per page
	                           </div>
	                           <div id=tripal_blast_report_pager>page
	                             <select id=tripal_blast_report_page_selector onChange="tripal_update_best_hit_report(this,'.$analysis_id.','.$sort.','.$descending.','.$per_page.')">';
   for ($i = 1; $i <= $no_page; $i ++) {
   	$html_out .= "<option value=$i>$i</option>";
   }	                           
   $html_out .=  '</select></div>
	                        </div>';
	return $html_out;
}