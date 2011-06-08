<?php
// $Id: views-view-table.tpl.php,v 1.8 2009/01/28 00:43:43 merlinofchaos Exp $
/**
 * @file views-view-table.tpl.php
 * Template to display a view as a table.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $header: An array of header labels keyed by field id.
 * - $fields: An array of CSS IDs to use for each field id.
 * - $class: A class or classes to apply to the table, based on settings.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 * - $rows: An array of row items. Each row is an array of content.
 *   $rows are keyed by row number, fields within rows are keyed by field ID.
 * @ingroup views_templates
 */
?>

<?php
$total = $view->total_rows;
$query = $view->build_info['query'];
$pager = $view->pager;
$limit = $pager ['items_per_page'];
$offset = $pager ['current_page'] * $limit;
$args = $view->build_info['query_args'];
global $base_url;
?>

<a id="tripal_search_unigene-result-top" name="search_results">
&nbsp;&nbsp;&nbsp;Your search produced <b><?php print $total ?></b> results<br>
<i>&nbsp;&nbsp;&nbsp;Note: To get complete annotation for a sequence, click on the sequence name.</i>
</a>
<table class="tripal_search_unigene-table <?php print $class; ?> tripal-table-horz">
  <?php if (!empty($title)) : ?>
    <caption><?php print $title; ?></caption>
  <?php endif; ?>
  <thead>
    <tr class="tripal_search_unigene-table-header tripal-table-header">
      <?php foreach ($header as $field => $label): ?>
        <th class="views-field views-field-<?php print $fields[$field]; ?>">
          <?php print $label; ?>
        </th>
      <?php endforeach; ?>
    </tr>
  </thead>
  <tbody>
    <?php foreach ($rows as $count => $row): 
    			$rowclass = "";
    			if ($count % 2 != 0) {
    				$rowclass = "tripal_search_unigene-table-even-row tripal-table-even-row";
    			} else {
    				$rowclass = "tripal_search_unigene-table-odd-row tripal-table-odd-row";
    			}
    ?>
      <tr class="<?php print $rowclass?> <?php print implode(' ', $row_classes[$count]); ?>">
        <?php foreach ($row as $field => $content): ?>
          <td class="views-field views-field-<?php print $fields[$field]; ?>">
            <?php print $content; ?>
          </td>
        <?php endforeach; ?>
      </tr>
    <?php endforeach; ?>
  </tbody>
</table>

<?php /* if ($count != 0) {
	// Generate Excel files
	// set parameters for excel file
	$param_excel= array();
	global $user;
	$dir = file_directory_path().'/tripal/tripal_search_unigene/'.session_id();
	if (!file_exists($dir)) {
		mkdir ($dir, 0777);
	}
	$file =session_id().'/unigene_search_result.xls';
	
	$param_excel['sheet_name']		= 'unigene_search';
	$path = "/tripal/tripal_search_unigene/$file";
	$param_excel['filename']		= file_directory_path().$path;
	$path_url =url("sites/default/files/tripal/tripal_search_unigene/$file");
	# set SQL
	$sql = "$query LIMIT $limit OFFSET $offset";
	# create heades
	$col_chr = 'A';
	$headers = array();
	$headers[$col_chr++] = array('align' => 'left', 'width' => 20, 'type' => 'hyperlink', 'field' => 'unigene_for_search_feature_name', 'heading' => 'Feature name');
	$headers[$col_chr++] = array('align' => 'left', 'width' => 10, 'type' => 'text', 'field' => 'unigene_for_search_organism_common_name', 'heading' => 'Organism');
	$headers[$col_chr++] = array('align' => 'left', 'width' => 10, 'type' => 'text', 'field' => 'unigene_for_search_feature_seqlen', 'heading' => 'Length');
	$headers[$col_chr++] = array('align' => 'left', 'width' => 10, 'type' => 'text', 'field' => 'unigene_for_search_feature_type', 'heading' => 'Type');
	$headers[$col_chr++] = array('align' => 'left', 'width' => 20, 'type' => 'text', 'field' => 'unigene_for_search_go_term', 'heading' => 'GO term');
	$headers[$col_chr++] = array('align' => 'left', 'width' => 20, 'type' => 'text', 'field' => 'unigene_for_search_blast_value', 'heading' => 'Blast');
	$headers[$col_chr++] = array('align' => 'left', 'width' => 20, 'type' => 'text', 'field' => 'unigene_for_search_kegg_value', 'heading' => 'KEGG');
	$headers[$col_chr++] = array('align' => 'left', 'width' => 20, 'type' => 'text', 'field' => 'unigene_for_search_interpro_value', 'heading' => 'Interpro');
	
	# populate data
	$result = chado_query($sql, $args);
	$data = array();
	while ($row = db_fetch_array($result)) {
		$data[] = array ('unigene_for_search_feature_name' => $row['unigene_for_search_feature_name'],
									 'hyperlink_unigene_for_search_feature_name' => "$base_url/ID".$row['unigene_for_search_feature_id'],
									 'unigene_for_search_organism_common_name' => $row['unigene_for_search_organism_common_name'],
									 'unigene_for_search_feature_seqlen' => $row['unigene_for_search_feature_seqlen'],
									 'unigene_for_search_feature_type' => $row['unigene_for_search_feature_type'],
									 'unigene_for_search_go_term' => $row['unigene_for_search_go_term'],
									 'unigene_for_search_blast_value' => $row['unigene_for_search_blast_value'],
									 'unigene_for_search_kegg_value' => $row['unigene_for_search_kegg_value'],
									 'unigene_for_search_interpro_value' => $row['unigene_for_search_interpro_value']
									 );
	}
	# add headers and data
	$param_excel['contents']['sheet']['headers']= $headers;
	$param_excel['contents']['sheet']['data']= $data;
	# generate excel
	ml_generate_excel($param_excel);
	print "&nbsp;&nbsp;&nbsp;<a href=\"$path_url\">Download as Excel file</a>";
	
}
*/?>

<?php 
# ----------------------------------------- #
# [UD] ml_generate_excel
# ----------------------------------------- #
function ml_generate_excel ($param_excel) {
	# get and update parameters
	$writer		= ($param_excel['version'] == '2007') ? 'Excel2007' : 'Excel5';
	$link_clr	= (empty($param_excel['link_clr'])) ? '0000FF' : $param_excel['link_clr'];
	$filename	= $param_excel['filename'];
	if (!preg_match("/(xls|xlsx)$/", $filename)) {
		$filename = ($param_excel['version'] == '2007') ? $filename.'.xlsx' : $filename.'.xls';
	}
	/*********************************************/
	# create an excel file
	/*********************************************/
	# create new PHPExcel object
	$excel = new PHPExcel();
	# Set properties
	$excel->getProperties()->setCreator("Main Lab");
	$excel->getProperties()->setLastModifiedBy("Main Lab");
	$excel->getProperties()->setTitle("Office 2007 XLSX Document");
	$excel->getProperties()->setSubject("");
	$excel->getProperties()->setKeywords("");
	$excel->getProperties()->setCategory("");
	$excel->getProperties()->setDescription("");
	# set default styles
	$excel->getDefaultStyle()->getFont()->setName('Times New Roman');
	$excel->getDefaultStyle()->getFont()->setSize(11);
	# create worksheets
	$sheet_no= 0;
	foreach ($param_excel['contents'] as $sheet_name => $sheet) {
		if ($sheet_no != 0) {
			# add a new worksheet
			$excel->createSheet();
		}	
		# set active sheet
		$excel->setActiveSheetIndex($sheet_no);		
		# get current worksheet
		$worksheet = $excel->getActiveSheet();	
		# set title for this sheet
		$worksheet->setTitle($sheet_name);
		# get headers
		$headers= $param_excel['contents'][$sheet_name]['headers'];
		# set style and headings for headers
		foreach ($headers as $col_chr => $prop) {
			# set alignment for column
			$align= PHPExcel_Style_Alignment::HORIZONTAL_LEFT;
			if ($prop['align'] == 'right') {
				$align= PHPExcel_Style_Alignment::HORIZONTAL_RIGHT;
			}
			else if ($prop['align'] == 'center') {
				$align= PHPExcel_Style_Alignment::HORIZONTAL_CENTER;
			}
			$worksheet->getStyle($col_chr.'1')->getAlignment()->setHorizontal($align);		
			# set width
			$worksheet->getColumnDimension($col_chr)->setWidth($prop['width']);
			$worksheet->getStyle($col_chr.'1')->getFont()->setBold(true);
			$worksheet->getStyle($col_chr.'1')->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID);
			$worksheet->getStyle($col_chr.'1')->getFill()->getStartColor()->setARGB('FFCDCDCD');
			$worksheet->setCellValue($col_chr.'1', $prop['heading']);
		}		
		# add data to current worksheet
		foreach ($param_excel['contents'][$sheet_name]['data'] as $idx => $row) {
			foreach ($headers as $col_chr => $prop) {
				$loc= $col_chr.($idx+2);
				# set alignment for cell
				$align= PHPExcel_Style_Alignment::HORIZONTAL_LEFT;
				if ($prop['align'] == 'right') {
					$align= PHPExcel_Style_Alignment::HORIZONTAL_RIGHT;
				}
				else if ($prop['align'] == 'center') {
					$align= PHPExcel_Style_Alignment::HORIZONTAL_CENTER;
				}
				$worksheet->getStyle($loc)->getAlignment()->setHorizontal($align);
				
				if (!empty($row[$prop['field']])) {
					# set cell value
					if ($prop['type'] == 'text') {
						$worksheet->setCellValue($loc, $row[$prop['field']]);
					}
					else if ($prop['type'] == 'hyperlink') {
						$worksheet->setCellValue($loc, $row[$prop['field']]);
						if ($row['hyperlink_'.$prop['field']]) {
							# add underline
							$worksheet->getStyle($loc)->getFont()->setUnderline(PHPExcel_Style_Font::UNDERLINE_SINGLE);
							# set link color
							$worksheet->getStyle($loc)->getFont()->getColor()->setRGB($link_clr);		
							# set URL
							$worksheet->getCell($loc)->getHyperlink()->setUrl($row['hyperlink_'.$prop['field']]);
						}
					}
				}
			}
		}
		$sheet_no++;
	}	
		# create excel file
 		$excel->setActiveSheetIndex(0);
 		$objWriter = PHPExcel_IOFactory::createWriter($excel, $writer);
 		$objWriter->save($filename);
}
?>