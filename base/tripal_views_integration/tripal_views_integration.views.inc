  <?php

  function tripal_views_integration_views_data(){
    //this gets rebuild on cache clear
    /*
    * note on fields: the handlers must be able to handle the type of the field
    * 									may be an issue without validation of data entry:
    * 									how the user associated fields with which handlers.
    */
/*
    $data['go_count_organism']['table']['group'] = t('go count organism');

    $data['go_count_organism']['table']['base'] = array(
    'field' => 'cvterm_id',
    'title' => t('go count organism table'),
    'help' => t("go count organism table contains mview data."), //garbage explanation for now
    );

    $data['go_count_organism']['cvname'] = array(
    'title' => t('cvname text field'),
    'help' => t(' text field.'),
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

    $data['go_count_organism']['cvterm_id'] = array(
    'title' => t('cvterm_id numeric field'),
    'help' => t(' numeric field.'),
    'field' => array(
    'handler' => 'views_handler_field_numeric',
    'click sortable' => TRUE,
    ),
    'filter' => array(
    'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
    'handler' => 'views_handler_sort',
    ),
    );

    $data['go_count_organism']['organism_id'] = array(
    'title' => t('organism_id numeric field'),
    'help' => t(' numeric field.'),
    'field' => array(
    'handler' => 'views_handler_field_numeric',
    'click sortable' => TRUE,
    ),
    'filter' => array(
    'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
    'handler' => 'views_handler_sort',
    ),
    );

    $data['go_count_organism']['feature_count'] = array(
    'title' => t('feature_count numeric field'),
    'help' => t(' numeric field.'),
    'field' => array(
    'handler' => 'views_handler_field_numeric',
    'click sortable' => TRUE,
    ),
    'filter' => array(
    'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
    'handler' => 'views_handler_sort',
    ),
    );
    

    $data['organism']['table']['join']['go_count_organism'] = array(
      'left_field' => 'organism_id',
      'field' => 'organism_id',
    );
*/
    

    $tvi_query = db_query('SELECT * FROM public.tripal_views_integration');

    //tvi = tripal_views_integration
    while($tvi_row = db_fetch_object($tvi_query)){

      //ids we'll use for queries
      $setup_id = $tvi_row->setup_id;
      $mview_id = $tvi_row->mview_id;

      //let's get the name of the table
      $mview_table = db_fetch_object(db_query("SELECT name, mv_specs FROM {tripal_mviews} WHERE mview_id = '$mview_id';"));

      //use name from above and description from $tvi_row
      $data[$mview_table->name]['table']['group'] = t($tvi_row->description);
      //TODO: this need  to be a base table! so need logic to figure out the primary key of this table, on the fly
      
      $data[$mview_table->name]['table']['base'] = array(
          'title' => t($tvi_row->name),
          'help' => t($tvi_row->description), //garbage explanation for now
      );

      //let's add fields
      //tmj = tripal_mviews_join
      $tmj_query = db_query("SELECT * FROM public.tripal_mviews_join WHERE setup_id = '$setup_id'");
      $i = 0;
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

        
        //TODO: get join info here per mview field
        
        $chado_join_field = $tmj_row->chado_table_join;
        $chado_join_column = $tmj_row->chado_column;
        $join_column = $tmj_row->view_column;
        
        $data[$chado_join_field]['table']['join'][$mview_table->name] = array(
        // Index this array by the table name to which this table refers.
        // 'left_field' is the primary key in the referenced table.
        // 'field' is the foreign key in this table.
          'left_field' => $join_column,
          'field' => $chado_join_column,
        );
        
        $i++;
      }


    }

    dpm("in tripal_views_integration file", 'dpm');

    dpm($data, 'data');
    return $data;
  }
