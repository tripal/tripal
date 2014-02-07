<?php
/**
 * @file
 * Functions for rendering term tree's
 */

/**
 * Renders the cv_list form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_show_browser() {

  $content = drupal_get_form('tripal_cv_list_form');
  $content .= "
    <div id=\"cv_browser\"></div>
  ";

  return $content;
}


/**
 * Generates JSON used for generating an exapandable tree of terms from
 * a controlled vocabulary that has associated counts.
 *
 * The progammer must first create a materialized view that
 * will generate count data for a given controlled vocabulary.  For example, the Tripal
 * Analysis GO creates a materialized view for counting Gene Ontology assignments
 * to features.  This view is created on install of the module and is named
 * 'go_count_analysis'.
 *
 * Second, the progammer must add an HTML 'div' box to the desired page
 * with a class name of 'tripal_cv_tree', and an id of the following format:
 *
 *  tripal_[module_name]_cv_tree_[unique id]
 *
 * where [module_name] is the name of the tripal module (e.g. tripal_analyisis_go)
 * and [unique id] is some unique identifier that the contolling module
 * recognizes. This string is the $tree_id variable passed as the first argument
 * to the function.  For example, the Tripal GO Analysis module generates
 * tree ids of the form:
 *
 *  tripal_analysis_go_cv_tree_10-2_bp
 *
 * In this case the module that will manage this tree is identified as 'tripal_analysis_go' and within
 * the [unique id] portion contains the
 * organism_id (e.g. 10), analysis_id (e.g. 2) and tree type (bp = biological process).
 *
 * Second, the programmer must then define a hook in the controlling module for setting
 * some options used to build the chart.  The hook has the form:  hook_cv_tree($tree_id).
 * This hook should accept the full $tree_id as the single parameter.  For the Tripal
 * Analysis GO module the hook is named:  tripal_analysis_go_cv_tree.
 *
 * The array returned by this hook must have the following fields:
 *  - cv_id
 *      the cv_id for the controlled vocabulary
 *  - count_mview
 *      the name of the materialized view that contains the count data
 *      this materialized view must have at least two columns, one with the cvterm_id
 *      for each term and a second column containing the counts. The tree will only
 *      be expanded to include child terms that have counts.
 *  - cvterm_id_column
 *      the column name in the materialized view that contains
 *      the cvterm_ids
 *  - count_column
 *      the column name in the materialized view that contains the
 *      counts
 *  - filter
 *      an SQL compatible WHERE clause string (whithout the word 'WHERE')
 *      that can be used for filtering the records in the materialized view.
 *  - label
 *      the title for the tree
 *
 * Example from the tripal_analysis_go module:
 * @code
 *  function tripal_analysis_go_cv_tree($tree_id) {
 *
 *   $organism_id = preg_replace("/^tripal_analysis_go_cv_tree_(\d+)-(\d+)_(bp|cc|mf)$/","$1",$tree_id);
 *   $analysis_id = preg_replace("/^tripal_analysis_go_cv_tree_(\d+)-(\d+)_(bp|cc|mf)$/","$2",$tree_id);
 *   $type        = preg_replace("/^tripal_analysis_go_cv_tree_(\d+)-(\d+)_(bp|cc|mf)$/","$3",$tree_id);
 *
 *   if(strcmp($type,'mf')==0) {
 *      $class = 'molecular_function';
 *   }
 *   if(strcmp($type,'cc')==0) {
 *      $class = 'cellular_component';
 *   }
 *   if(strcmp($type,'bp')==0) {
 *      $class = 'biological_process';
 *   }
 *
 *   $options = array(
 *      cv_id            => tripal_cv_get_cv_id($class),
 *      count_mview      => 'go_count_analysis',
 *      cvterm_id_column => 'cvterm_id',
 *      count_column     => 'feature_count',
 *      filter           => "CNT.organism_id = $organism_id AND CNT.analysis_id = $analysis_id",
 *      label            => 'Features',
 *   );
 *   return $options;
 * }
 *
 * @endcode
 *
 * @param $tree_id
 *   The unique identifier for the tree
 *
 * @return
 *   JSON array needed for the jsTree package for generating expandable trees.
 *
 * With these three components (materialized view, a 'div' box with proper CSS class and ID, and a hook_cv_tree)
 * a tree will be created on the page.  There is no need to call this function directly.
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_tree($tree_id) {
  // parse out the tripal module name from the chart_id to find out
  // which Tripal "hook" to call:
  $tripal_mod = preg_replace("/^(tripal_.+?)_cv_tree_(.+)$/", "$1", $tree_id);
  if ($tripal_mod) {
    $callback = $tripal_mod . "_cv_tree";

    // now call the function in the module responsible for the tree.  This
    // should call the tripal_cv_init_cv with the proper parameters set for
    // getting the cv_id of the vocabulary to use
    $opt = call_user_func_array($callback, array($tree_id));

    // we only need to return the cv_id for this function call.
    $json_array[] = $opt['cv_id'];
  }

  $json_array[] = $tree_id;
  return drupal_json($json_array);
}

/**
 * Update the JSON tree describing a tree
 *
 * @ingroup tripal_cv
 */
function tripal_cv_update_tree() {
  $content = array();
  $ontology = 'sequence';

  // get the id of the term to look up
  $cv = check_plain($_REQUEST['cv']);
  $term = check_plain($_REQUEST['term']);
  $tree_id = check_plain($_REQUEST['tree_id']);

  // get the options needed for this tree from the tripal module that
  // wants to create the tree
  $tripal_mod = preg_replace("/^(tripal_.+?)_cv_tree_(.+)$/", "$1", $tree_id);
  if ($tripal_mod) {
    $callback = $tripal_mod . "_cv_tree";
    $opt = call_user_func_array($callback, array($tree_id));
  }

  // get the CV root terms
  if (strcmp($term, 'root')==0) {
    if (!$cv) {
      $cv = $opt['cv_id'];
    }
    $content = tripal_cv_init_tree($cv, $opt['count_mview'],
      $opt['cvterm_id_column'], $opt['count_column'], $opt['filter'], $opt['label']);

  // get the children terms
  }
  else {
    $content = tripal_cv_get_term_children($term, $opt['count_mview'],
      $opt['cvterm_id_column'], $opt['count_column'], $opt['filter'], $opt['label']);
  }

  drupal_json($content);
}

/**
 * Generates JSON needed for jsTree Root-level Branches
 *
 * This function returns the JSON array for the jsTree
 *    jQuery code that builds a tree for browsing the ontology.  This function
 *    should be called to generate the root level branches of the tree.
 *
 * @ingroup tripal_cv
 */
function tripal_cv_init_tree($cv_id, $cnt_table = NULL, $fk_column = NULL,
  $cnt_column = NULL, $filter = NULL, $label = NULL) {

  // get the list of root terms for the provided CV
  $sql = "
    SELECT *
    FROM {cv_root_mview} CRM
    WHERE cv_id = :cv_id
  ";
  $results = chado_query($sql, array(':cv_id' => $cv_id));

  // prepare the SQL statement that will allow us to pull out count
  // information for each term in the tree.
  if ($cnt_table) {
    if (!$filter) {
      $filter = '(1=1)';
    }
    $cnt_sql = "
       SELECT CVT.name, CVT.cvterm_id, CNT.$cnt_column as num_items
       FROM {$cnt_table} CNT
        INNER JOIN {cvterm} CVT on CNT.$fk_column = CVT.cvterm_id
       WHERE $filter AND CVT.cvterm_id = :cvterm_id
       ORDER BY $cnt_column desc
    ";
  }

  foreach ($results as $term) {
    $name = $term->name;
    $count = 0;
    if ($cnt_table) {
      $cnt_results = chado_query($cnt_sql, array(':cvterm_id' => $term->cvterm_id));
      foreach ($cnt_results as $cnt) {
        $count += $cnt->cnt;
      }
      if ($count > 0) {
        $name .= " ($count $label(s))";
      }
    }
    $content[] = array(
      'attributes' => array(
        'id' => $term->cvterm_id,
      ),
      'state' => 'closed',
      'data' => $name,
      'children' => array(),
    );
  }

  return $content;

}

/**
 *  Generates JSON needed for jsTree -expanding a term to view children
 *
 *  This function returns the JSON array for the jsTree
 *    jQuery code when expanding a term to view it's children.
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_term_children($cvterm_id, $cnt_table = NULL,
  $fk_column = NULL, $cnt_column = NULL, $filter = NULL, $label = NULL) {

  // get the children for the term provided
  $sql = "
    SELECT CVTR.cvterm_relationship_id,CVTR.subject_id,
       CVT1.name as subject_name, CVT3.name as type_name, CVTR.type_id,
       CVT2.name as object_name,CVTR.object_id
    FROM {cvterm_relationship} CVTR
       INNER JOIN {CVTerm} CVT1 on CVTR.subject_id = CVT1.cvterm_id
       INNER JOIN {CVTerm} CVT2 on CVTR.object_id = CVT2.cvterm_id
       INNER JOIN {CVTerm} CVT3 on CVTR.type_id = CVT3.cvterm_id
       INNER JOIN {CV} on CV.cv_id = CVT1.cv_id
    WHERE CVTR.object_id = :cvterm_id
    ORDER BY CVT1.name
  ";
  $results = chado_query($sql, array(':cvterm_id' => $cvterm_id));

  // prepare the SQL statement that will allow us to pull out count
  // information for each term in the tree.
  if ($cnt_table) {
    if (!$filter) {
      $filter = '(1=1)';
    }
    $cnt_sql = "
       SELECT CVT.name, CVT.cvterm_id, CNT.$cnt_column as num_items
       FROM {$cnt_table} CNT
        INNER JOIN {cvterm} CVT on CNT.$fk_column = CVT.cvterm_id
       WHERE $filter AND CVT.cvterm_id = :cvterm_id
       ORDER BY $cnt_column desc
    ";
  }

  // populate the JSON content array
  foreach ($results as $term) {
    // count the number of items per term if requested
    $name = $term->subject_name;
    $count = 0;
    if ($cnt_table) {
      $cnt_results = chado_query($cnt_sql, array(':cvterm_id' => $term->subject_id));
      foreach ($cnt_results as $cnt) {
        $count += $cnt->num_items;
      }
      if ($count > 0) {
        $name .= " (" . number_format($count) . " $label)";

        // check if we have any children if so then set the value
        $children = chado_query($sql, array(':cvterm_id' => $term->subject_id))->fetchObject();
        $state = 'leaf';
        if ($children) {
          $state = 'closed';
        }
        $content[] = array(
          'attributes' => array(
            'id' => $term->subject_id,
          ),
          'state' => $state,
          'data' => $name,
          'children' => array(),
        );
      }
    }
    else {
      // check if we have any children if so then set the value
      $children = chado_query($sql, array(':cvterm_id' => $term->subject_id))->fetchObject();
      $state = 'leaf';
      if ($children) {
        $state = 'closed';
      }
      $content[] = array(
        'attributes' => array(
           'id' => $term->subject_id,
        ),
        'state' => $state,
        'data' => $name,
        'children' => array(),
      );
    }
  }
  $content[] = $cnt_sql;

  return $content;
}

/**
 * Initilizes term tree browser
 *
 * @ingroup tripal_cv
 */
function tripal_cv_init_browser($cv_id) {

  $content = "
    <div id=\"tripal_cv_cvterm_info_box\">
      <a href=\"#\" onclick=\"$('#tripal_cv_cvterm_info_box').hide()\" style=\"float: right\">Close [X]</a>
      <h3>Term Information</h3>
      <div id=\"tripal_cv_cvterm_info\"></div>
    </div>
    <div id=\"tripal_ajaxLoading\" style=\"display:none\">
      <div id=\"loadingText\">Loading...</div>
      <img src=\"$url\">
    </div>
    <h3>Tree Browser</h3>
    <div id=\"browser\"</div>
    </div>
  ";

  drupal_json(array('update' => "$content"));

}

/**
 * Describe a cvterm (Rendered)
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_info($cvterm_id) {

  // get the id of the term to look up
  $cv = check_plain($_REQUEST['cv']);
  $tree_id = check_plain($_REQUEST['tree_id']);

  // first get any additional information to add to the cvterm
  if (strcmp($tree_id, 'undefined') != 0) {
    $tripal_mod = preg_replace("/^(tripal_.+?)_cv_tree_(.+)$/", "$1", $tree_id);
    if ($tripal_mod) {
      $callback = $tripal_mod . "_cvterm_add";
      $opt = call_user_func_array($callback, array($cvterm_id, $tree_id));
    }
  }

  $sql = "
    SELECT CVT.name as cvtermname, CVT.definition, CV.name as cvname,
       DBX.accession,DB.urlprefix,DB.db_id,DB.name as dbname
    FROM {CVTerm} CVT
      INNER JOIN {CV} on CVT.cv_id = CV.cv_id
      INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
      INNER JOIN {DB} on DBX.db_id = DB.db_id
    WHERE CVT.cvterm_id = :cvterm_id
  ";
  $cvterm = chado_query($sql, array(':cvterm_id' => $cvterm_id))->fetchObject();

  $sql = "
    SELECT CVTS.synonym, CVT.name as cvname
    FROM {cvtermsynonym} CVTS
      INNER JOIN {cvterm} CVT on CVTS.type_id = CVT.cvterm_id
    WHERE CVTS.cvterm_id = :cvterm_id

  ";
  $results = chado_query($sql, array(':cvterm_id' => $cvterm_id));
  foreach ($results as $synonym) {
    $synonym_rows .= "<b>$synonym->cvname:</b>  $synonym->synonym<br />";
  }

  $accession = $cvterm->accession;
  if ($cvterm->urlprefix) {
    $accession = "<a href=\"$cvterm->urlprefix$cvterm->accession\">$cvterm->accession</a>";
  }

  $content = "
    <div id=\"cvterm\">
    <table>
      <tr><th>Term</th><td>$cvterm->cvtermname</td></tr>
      <tr><th>Accession</th><td>$accession</td></tr>
      <tr><th>Ontology</th><td>$cvterm->cvname</td></tr>
      <tr><th>Definition</th><td>$cvterm->definition</td></tr>
      <tr><th>Synonyms</th><td>$synonym_rows</td></tr>
      <tr><th>Internal ID</th><td>$cvterm_id</td></tr>
  ";

  // now add in any additional options from a hook
  if ($opt) {
    foreach ($opt as $key => $value) {
      $content .= "<tr><th>$key</th><td>$value</td>";
    }
  }

  // close out the information table
  $content .= "
    </table>
    </div>
  ";
  drupal_json(array('update' => $content));
}

/**
 * Form listing CVs
 *
 * @ingroup tripal_cv
 */
function tripal_cv_list_form($form_state) {

  // get a list of db from chado for user to choose
  $sql = "
    SELECT DISTINCT CV.name,CV.cv_id
    FROM {cvterm_relationship} CVTR
       INNER JOIN {cvterm} CVT on CVTR.object_id = CVT.cvterm_id
       INNER JOIN {CV} on CV.cv_id = CVT.cv_id
  ";
  $results = chado_query($sql);

  $blastdbs = array();
  $cvs[''] = '';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }

  $form['db_options'] = array(
    '#type' => 'value',
    '#value' => $cvs
  );

  $form['cv_list'] = array(
    '#title' => t('CVs with relationships'),
    '#type' => 'select',
    '#description' => t('Choose the controlled vocabulary to browse'),
    '#options' => $form['db_options']['#value'],
    '#attributes' => array(
      'onChange' => "return tripal_cv_init_browser(this)",
    )
  );

  return $form;
}
