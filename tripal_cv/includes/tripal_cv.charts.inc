<?php
/**
 * @file
 * Tripal API for generating a Google Chart of count data
 */

/**
 * Generates JSON used for generating a Google chart of count data associated
 * with a controlled vocabulary.  An example would be features assigned to
 * Gene Ontology terms.
 *
 * To generate a chart, the progammer must first create a materialized view that
 * will generate count data for a given controlled vocabulary.  For example, the Tripal
 * Analysis GO creates a materialized view for counting Gene Ontology assignments
 * to features.  This view is created on install of the module and is named
 * 'go_count_analysis'.
 *
 * Next, an HTML 'div' box must be added to the desired page
 * with a class name of 'tripal_cv_chart', and an id of the following format:
 *
 *  tripal_[module_name]_cv_chart_[unique id]
 *
 * where [module_name] is the name of the tripal module (e.g. tripal_analyisis_go)
 * and [unique id] is some unique identifier that the contolling module
 * recognizes. This string is the $chart_id variable passed as the first argument
 * to the function.  For example, the Tripal GO Analysis module generates
 * chart ids of the form:
 *
 *  tripal_analysis_go_cv_chart_10_2_bp
 *
 * In this case the module that will manage this chart is identified as 'tripal_analysis_go' and within
 * the [unique id] portion contains the
 * organism_id (e.g. 10), analysis_id (e.g. 2) and chart type (bp = biological process).
 *
 * Second, the programmer must then define a hook in the controlling module for setting
 * some options used to build the chart.  The hook has the form:  hook_cv_chart($chart_id).
 * This hook should accept the full $chart_id as the single parameter.  For the Tripal
 * Analysis GO module the hook is named:  tripal_analysis_go_cv_chart.
 *
 * The array returned by this hook must have the following fields:
 *  - count_mview
 *      the name of the materialized view that contains the count data
 *      this materialized view must have at least two columns, one with the cvterm_id
 *      for each term and a second column containing the counts
 *  - cvterm_id_column
 *      the column name in the materialized view that contains
 *      the cvterm_ids
 *  - count_column
 *      the column name in the materialized view that contains the
 *      counts
 *  - filter
 *      an SQL compatible WHERE clause string (whithout the word 'WHERE')
 *      that can be used for filtering the records in the materialized view.
 *  - title
 *      the title for the chart
 *  - type
 *      the type of chart to create (see Google Charts documenation). Leave
 *      blank for a pie chart.
 *  - size
 *      the dimensions of the chart in pixels (see Google Charts documenations
 *      for exact size limitations).
 *
 * Example from the tripal_analysis_go module:
 * @code
 *  function tripal_analysis_go_cv_chart($chart_id) {
 *
 *    // The CV module will create the JSON array necessary for buillding a
 *    // pie chart using jgChart and Google Charts.  We have to pass to it
 *    // a table that contains count information, tell it which column
 *    // contains the cvterm_id and provide a filter for getting the
 *    // results we want from the table.
 *    $organism_id = preg_replace("/^tripal_analysis_go_cv_chart_(\d+)-(\d+)_(bp|cc|mf)$/","$1",$chart_id);
 *    $analysis_id = preg_replace("/^tripal_analysis_go_cv_chart_(\d+)-(\d+)_(bp|cc|mf)$/","$2",$chart_id);
 *    $type        = preg_replace("/^tripal_analysis_go_cv_chart_(\d+)-(\d+)_(bp|cc|mf)$/","$3",$chart_id);
 *
 *    $sql = "SELECT * FROM {analysis} WHERE analysis_id = :analysis_id";
 *    $analysis = chado_query($sql, array(':analysis_id' => $analysis_id))->fetchObject();
 *
 *    if (strcmp($type,'mf')==0) {
 *       $class = 'molecular_function';
 *       $title = "Number of Molecular Function Terms From $analysis->name Analysis";
 *    }
 *    if (strcmp($type,'cc')==0) {
 *       $class = 'cellular_component';
 *       $title = "Number of Cellular Component Terms From $analysis->name Analysis";
 *    }
 *    if (strcmp($type,'bp')==0) {
 *       $class = 'biological_process';
 *       $title = "Number of Biological Process Terms From $analysis->name Analysis";
 *    }
 *    $options = array(
 *       count_mview      => 'go_count_analysis',
 *       cvterm_id_column => 'cvterm_id',
 *       count_column     => 'feature_count',
 *       filter           => "
 *          CNT.organism_id = $organism_id AND
 *          CNT.analysis_id = $analysis_id AND
 *          CNT.cvterm_id IN (
 *            SELECT CVTR.subject_id
 *            FROM {cvterm_relationship} CVTR
 *              INNER JOIN {CVTerm} CVT on CVTR.object_id = CVT.cvterm_id
 *              INNER JOIN {CV} on CVT.cv_id = CV.cv_id
 *            WHERE CVT.name = '$class' AND
 *                   CV.name = '$class'
 *          )
 *       ",
 *       type             => 'p',
 *       size             => '550x175',
 *       title            => $title,
 *    );
 *    return $options;
 *  }
 *
 * @endcode
 *
 * @param $chart_id
 *   The unique identifier for the chart
 *
 * @return
 *   JSON array needed for the js caller
 *
 * With these three components (materialized view, a 'div' box with proper CSS class and ID, and a hook_cv_chart)
 * a chart will be created on the page.  There is no need to call this function directly.
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_chart($chart_id) {
  // parse out the tripal module name from the chart_id to find out
  // which Tripal "hook" to call:
  $tripal_mod = preg_replace("/^(tripal_.+?)_cv_chart_(.+)$/", "$1", $chart_id);
  $callback = $tripal_mod . "_cv_chart";

  // now call the function in the module responsible for the chart to fill out
  // an options array needed by the tripal_cv_count_chart call below.
  $opt = call_user_func_array($callback, array($chart_id));

  // build the JSON array to return to the javascript caller
  $json_arr = tripal_cv_count_chart(
    $opt['count_mview'],
    $opt['cvterm_id_column'],
    $opt['count_column'],
    $opt['filter'],
    $opt['title'],
    $opt['type'],
    $opt['size']
  );
  $json_arr[] = $chart_id;  // add the chart_id back into the json array

  return drupal_json($json_arr);

}

 /**
  * This function generates an array with fields compatible with Google charts used
  * for generating pie charts of counts associated with terms in
  * a controlled vocabulary.  An example would be counts of features assigned
  * to terms in the Sequence Ontology.
  *
  * @param $cnt_table
  *   The name of the table (most likely a materialized view) that
  *   contains count data for each term.  The table must have at least
  *   two columns, one with the cvterm_id for each term, and a second
  *   column with the count (i.e. features assigned the term).
  * @param $fk_column
  *   This is the name of the column in the $cnt_table that holds the
  *   cvterm_id for each term.
  * @param $cnt_column
  *   The name of the column in the $cnt_table containing the counts
  * @param $filter
  *   An SQL compatible 'where' clause (without the word 'WHERE') used
  *   to filter the records in the $cnt_table
  * @param $title
  *   The title of the chart to be rendered.
  * @param $type
  *   The type of chart to be rendered. The value used here is the same as
  *   the type names for Google charts. Default is p3 (pie chart).
  * @param $size
  *   The size in pixels of the chart to be rendered. Default is 300x75. The
  *   size of the chart is constrained by Google charts.  See the Google
  *   chart documentation for exact limitations.
  *
  * @return
  *   An array that has the settings needed for Google Charts to creat the chart.
  *
  * @ingroup tripal_cv_api
  */
function tripal_cv_count_chart($cnt_table, $fk_column,
  $cnt_column, $filter = NULL, $title = '', $type = 'p3', $size='300x75') {

  if (!$type) {
    $type = 'p3';
  }

  if (!$size) {
    $size = '300x75';
  }

  if (!$filter) {
    $filter = '(1=1)';
  }

  $is_pie = 0;
  if (strcmp($type, 'p') == 0 or strcmp($type, 'p3') == 0) {
    $is_pie = 1;
  }
  $sql = "
    SELECT CVT.name, CVT.cvterm_id, CNT.$cnt_column as num_items
    FROM {$cnt_table} CNT
     INNER JOIN {cvterm} CVT on CNT.$fk_column = CVT.cvterm_id
    WHERE $filter
  ";

  $features = array();
  $results = chado_query($sql);
  $data = array();
  $axis = array();
  $legend = array();
  $total = 0;
  $max = 0;
  $i = 1;
  while ($term = db_fetch_object($results)) {

    if ($is_pie) {
      $axis[] = "$term->name (" . number_format($term->num_items) . ")";
      $data[] = array($term->num_items, 0, 0);
    }
    else {
      $axis[] = "$term->name (" . number_format($term->num_items) . ")";
      $data[] = array($term->num_items);
      //$legend[] = "$term->name (" . number_format($term->num_items) . ")";
    }
    if ($term->num_items > $max) {
      $max = $term->num_items;
    }
    $total += $term->num_items;
    $i++;
  }
  // convert numerical values into percentages
  foreach ($data as &$set) {
    $set[0] = ($set[0] / $total) * 100;
  }
  $opt[] = array(
    data => $data,
    axis_labels => $axis,
    legend => $legend,
    size => $size,
    type => $type,

    bar_width     => 10,
    bar_spacing   => 0,
    title         => $title
  );
  //   $opt[] = $sql;

  return $opt;
}
