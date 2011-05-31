<?php


/**
 * Generates JSON used for generating a chart
 *
 * @param $chart_id
 *   The unique identifier for the chart
 *
 * @return
 *   JSON array needed for the js caller
 *
 * @ingroup tripal_cv
 */
function tripal_cv_chart($chart_id){
  // parse out the tripal module name from the chart_id to find out 
  // which Tripal "hook" to call:
  $tripal_mod = preg_replace("/^(tripal_.+?)_cv_chart_(.+)$/","$1",$chart_id);
  $callback = $tripal_mod . "_cv_chart";

  // now call the function in the module responsible for the chart.  This 
  // should call the tripal_cv_count_chart with the proper parameters set
  $opt = call_user_func_array($callback,array($chart_id));

  // build the JSON array to return to the javascript caller
  $json_arr = tripal_cv_count_chart($opt[count_mview],$opt[cvterm_id_column],
     $opt[count_column],$opt[filter],$opt[title], $opt[type],$opt[size]);
  $json_arr[] = $chart_id;  // add the chart_id back into the json array

  return drupal_json($json_arr);

}

 /**
  * Determines the counts needed for the chart to be rendered
  *
  * @param $cnt_table
  *   The table containing counts for the various cvterms
  * @param $fk_column
  *   The column in the count table to join it to the cvterm table
  * @param $cnt_column
  *   The name of the column in the count table containing the counts
  * @param $filter
  *   A Filter string. Default is (1=1).
  * @param $title
  *   The title of the chart to be rendered.
  * @param $type
  *   The type of chart to be rendered. Default is p3 (pie chart).
  * @param $size
  *   The size of the chart to be rendered. Default is 300x75.
  *
  * @return 
  *   An options array needed to render the chart specified
  *
  * @ingroup tripal_cv_api
  */
function tripal_cv_count_chart($cnt_table, $fk_column,
   $cnt_column, $filter = null, $title = '', $type = 'p3', $size='300x75') {

   if(!$type){
      $type = 'p3';
   }

   if(!$size){
     $size = '300x75';
   }

   if(!$filter){
      $filter = '(1=1)'; 
   }

   $isPie = 0;
   if(strcmp($type,'p')==0 or strcmp($type,'p3')==0){
      $isPie = 1;
   }
   $sql = "
      SELECT CVT.name, CVT.cvterm_id, CNT.$cnt_column as num_items
      FROM {$cnt_table} CNT 
       INNER JOIN {cvterm} CVT on CNT.$fk_column = CVT.cvterm_id 
      WHERE $filter
   ";    

   $features = array();
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $results = db_query($sql);
   tripal_db_set_active($previous_db);  // now use drupal database
   $data = array();
   $axis = array();
   $legend = array();
   $total = 0;
   $max = 0;
   $i = 1;
   while($term = db_fetch_object($results)){
      
      if($isPie){
         $axis[] = "$term->name (".number_format($term->num_items).")";
         $data[] = array($term->num_items,0,0);
      } else {
         $axis[] = "$term->name (".number_format($term->num_items).")";
         $data[] = array($term->num_items);
    //     $legend[] = "$term->name (".number_format($term->num_items).")";
      }
      if($term->num_items > $max){
         $max = $term->num_items;
      }
      $total += $term->num_items;
      $i++;
   }
   // convert numerical values into percentages
   foreach($data as &$set){
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
