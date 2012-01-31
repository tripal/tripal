  <?php
/**
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_views_data(){   

    $tvi_query = db_query('SELECT * FROM tripal_views_integration');

    //tvi = tripal_views_integration
    while($tvi_row = db_fetch_object($tvi_query)){

      //ids we'll use for queries
      $setup_id = $tvi_row->setup_id;
      $mview_id = $tvi_row->mview_id;

      //let's get the name of the table
      $mview_table = db_fetch_object(db_query("SELECT name, mv_specs FROM {tripal_mviews} WHERE mview_id = '$mview_id';"));

      //use name from above and description from $tvi_row
      $data[$mview_table->name]['table']['group'] = t('Mview ' . $tvi_row->name);
      $data[$mview_table->name]['table']['base'] = array(
        'group' => t($tvi_row->name),
        'title' => t($tvi_row->name),
        'help'  => t($tvi_row->description),
      );

      //let's add fields
      //tmj = tripal_mviews_join
      $tmj_query = db_query("SELECT * FROM tripal_mviews_join WHERE setup_id = '$setup_id'");
      while($tmj_row = db_fetch_object($tmj_query)){

        $column_name = $tmj_row->view_column;
        $handlers = db_fetch_object(db_query("SELECT handler_filter, handler_field FROM {tripal_views_handlers} WHERE setup_id = '$setup_id' AND column_name = '$column_name';"));
        //handlers would be used $handlers->handler_filter, $handlers->handler_field etc, thuogh may need to include new ones in this query or do select *

        //let's use handlers we retrieved from above
        $data[$mview_table->name][$tmj_row->view_column] = array(
           'title' => t($tmj_row->view_column),
           'help' => t("**"),
           'field' => array(
              'handler' => 'views_handler_field',
              'click sortable' => TRUE,
           ),
           'sort' => array(
              'handler' => 'views_handler_sort',
           ),
           'filter' => array(
              'handler' => 'views_handler_filter_string',
           ),
           'argument' => array(
              'handler' => 'views_handler_argument_string',
           ),
        );
               
        $chado_join_table = $tmj_row->chado_table_join;
        $chado_join_column = $tmj_row->chado_column;
        $mview_join_column = $tmj_row->view_column;
        
        // add recipricol join entries
        $data["$mview_table->name"]['table']['join']["$chado_join_table"] = array(
          'left_field' => $chado_join_column,
          'field' => $mview_join_column,
        );

        // check to see if this table is one that correlates with Drupal nodes
        // if so, there will be a chado_<table_name> table in the Drupal database
        // if there is, then we need to add the linking join information
        $sql = "SELECT tablename FROM pg_tables WHERE tablename = 'chado_$chado_join_table'";
        if(db_fetch_object(db_query($sql))){

           // join the mview to the linker table
           $data["$mview_table->name"]['table']['join']["chado_$chado_join_table"] = array(
             'left_field' => $chado_join_column,
             'field' => $mview_join_column,
           );
        }
      }
    }

    return $data;
}
/**      $setup_id = $tvi_row->setup_id;
 *
 * @ingroup tripal_views_integration
 */
function tripal_views_integration_views_data_alter(&$data) {
    $tvi_query = db_query('SELECT * FROM tripal_views_integration');

    //tvi = tripal_views_integration
    while($tvi_row = db_fetch_object($tvi_query)){

      //ids we'll use for queries
      $mview_id = $tvi_row->mview_id;
      $setup_id = $tvi_row->setup_id;

      //let's get the name of the table
      $mview_table = db_fetch_object(db_query("SELECT name, mv_specs FROM {tripal_mviews} WHERE mview_id = '$mview_id';"));

      // iterate through the columns and alter the existing data array for
      // joins to other tables
      $tmj_query = db_query("SELECT * FROM tripal_mviews_join WHERE setup_id = '$setup_id'");
      while($tmj_row = db_fetch_object($tmj_query)){
        
        $chado_join_table = $tmj_row->chado_table_join;
        $chado_join_column = $tmj_row->chado_column;
        $mview_join_column = $tmj_row->view_column;      
 
        // add the recipricol join entries for each column
        $data["$chado_join_table"]['table']['join']["$mview_table->name"] = array(
          'left_field' => $mview_join_column,
          'field' => $chado_join_column,
        );

        // check to see if this table is one that correlates with Drupal nodes
        // if so, there will be a chado_<table_name> table in the Drupal database
        // if there is, then we need to add the linking join information.  We did
        // this step in the hook_views_data function above, but now we need 
        // to add the reciprical joins
        $sql = "SELECT tablename FROM pg_tables WHERE tablename = 'chado_$chado_join_table'";
        if(db_fetch_object(db_query($sql))){

           // join the linker table to the mview
           $data["chado_$chado_join_table"]['table']['join']["$mview_table->name"] = array(
             'left_field' => $mview_join_column,
             'field' => $chado_join_column,
           );

           // Join the node table to the view by way of the chado linker table
           $data['node']['table']['join']["$mview_table->name"] = array(
           	'left_table' => "chado_$chado_join_table",
           	'left_field' => 'nid',
           	'field' => 'nid',
           );
        }
      }
    }

    return $data;
}
