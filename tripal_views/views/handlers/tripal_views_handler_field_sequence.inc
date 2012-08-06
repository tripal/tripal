<?php

/**
 * @file
 * A chado wrapper for the views_handler_field.
 *
 * Handles display of sequence data.  If will aggregate sequences that need
 * to be aggregated (e.g. coding sequences) and provide 
 */
class tripal_views_handler_field_sequence extends chado_views_handler_field {

  function init(&$view, $options) {
    parent::init($view, $options);
    
    // to speed things up we need to make sure we have a persistent connection
    tripal_db_persistent_chado(); 
    
    if (!tripal_core_is_sql_prepared('sequence_by_parent')) {
      // prepare the queries we're going to use later during the render phase
      // This SQL statement uses conditionals in the select clause to handle
      // cases cases where the alignment is in the reverse direction and when
      // the upstream and downstream extensions go beyond the lenght of the 
      // parent sequence.
      $psql ='PREPARE sequence_by_parent (int, int, int) AS 
              SELECT
                OF.name srcname, FL.srcfeature_id, FL.strand, OCVT.name as srctypename, SCVT.name as typename,
                FL.fmin, FL.fmax,
                CASE 
                  WHEN FL.strand >= 0 THEN 
                    CASE 
                       WHEN FL.fmin - $1 <= 0 THEN 0
                       ELSE FL.fmin - $1
                    END
                  WHEN FL.strand < 0 THEN
                    CASE 
                       WHEN FL.fmin - $2 <= 0 THEN 0
                       ELSE FL.fmin - $2
                    END                   
                END as adjfmin,                                
                                
                CASE 
                  WHEN FL.strand >= 0 THEN
                    CASE 
                      WHEN FL.fmax + $2 > OF.seqlen THEN OF.seqlen 
                      ELSE FL.fmax + $2
                    END
                  WHEN FL.strand < 0 THEN
                    CASE
                      WHEN FL.fmax + $1 > OF.seqlen THEN OF.seqlen
                      ELSE FL.fmax + $1   
                    END               
                END as adjfmax,     
                
                CASE 
                  WHEN FL.strand >= 0 THEN 
                    CASE 
                       WHEN FL.fmin - $1 <= 0 THEN FL.fmin
                       ELSE $1
                    END
                  ELSE
                    CASE 
                       WHEN FL.fmax + $1 > OF.seqlen THEN OF.seqlen - FL.fmax
                       ELSE $1
                    END                   
                END as upstream,
                
                CASE 
                  WHEN FL.strand >= 0 THEN 
                    CASE 
                       WHEN FL.fmax + $2 > OF.seqlen THEN OF.seqlen - FL.fmax
                       ELSE $2
                    END
                  ELSE
                    CASE 
                       WHEN FL.fmin - $2 <= 0 THEN FL.fmin
                       ELSE $2
                    END                   
                END as downstream,            
                              
                CASE 
                  WHEN FL.strand >= 0 THEN 
                    CASE 
                       WHEN FL.fmin - $1 <= 0 THEN substring(OF.residues from 1 for ((FL.fmax - FL.fmin) + $1 + $2)) 
                       ELSE substring(OF.residues from (FL.fmin + 1 - $1) for ((FL.fmax - FL.fmin) + $1 + $2))
                    END
                  WHEN FL.strand < 0 THEN
                    CASE 
                      WHEN FL.fmin - $2 <= 0 THEN substring(OF.residues from 1 for ((FL.fmax - FL.fmin) + $1 + $2)) 
                      ELSE substring(OF.residues from (FL.fmin + 1 - $2) for ((FL.fmax - FL.fmin) + $1 + $2))      
                    END
                END as residues
              FROM featureloc FL 
                INNER JOIN feature SF on FL.feature_id = SF.feature_id
                INNER JOIN cvterm SCVT on SF.type_id = SCVT.cvterm_id
                INNER JOIN feature OF on FL.srcfeature_id = OF.feature_id                
                INNER JOIN cvterm OCVT on OF.type_id = OCVT.cvterm_id
              WHERE SF.feature_id = $3';
              
      $status = chado_query($psql);
      if (!$status) {
        watchdog('tripal_views_handler_field_sequence', 
          "init: not able to prepare SQL statement '%name'", 
          array('%name' => 'sequence_by_parent'), 'WATCHDOG ERROR');
      }
      // this query is meant to get all of the sub features of any given
      // feature (arg #1) and order them as they appear on the reference
      // feature (arg #2).
      $psql ='PREPARE sub_features (int, int) AS 
              SELECT SF.feature_id, CVT.name as type_name, SF.type_id
              FROM feature_relationship FR
                INNER JOIN feature SF on SF.feature_id = FR.subject_id
                INNER JOIN cvterm CVT on CVT.cvterm_id = SF.type_id
                INNER JOIN featureloc FL on FL.feature_id = FR.subject_id
                INNER JOIN feature PF on PF.feature_id = FL.srcfeature_id
              WHERE FR.object_id = $1 and PF.feature_id = $2
              ORDER BY FL.fmin ASC';
              
      $status = chado_query($psql);
      if (!$status) {
        watchdog('tripal_views_handler_field_sequence', 
          "init: not able to prepare SQL statement '%name'", 
          array('%name' => 'ssub_features'), 'WATCHDOG ERROR');
      }
      $psql ='PREPARE count_sub_features (int, int) AS 
              SELECT count(*) as num_children
              FROM feature_relationship FR
                INNER JOIN feature SF on SF.feature_id = FR.subject_id
                INNER JOIN cvterm CVT on CVT.cvterm_id = SF.type_id
                INNER JOIN featureloc FL on FL.feature_id = FR.subject_id
                INNER JOIN feature PF on PF.feature_id = FL.srcfeature_id
              WHERE FR.object_id = $1 and PF.feature_id = $2';
              
      $status = chado_query($psql);
      if (!$status) {
        watchdog('tripal_views_handler_field_sequence', 
          "init: not able to prepare SQL statement '%name'", 
          array('%name' => 'count_sub_features'), 'WATCHDOG ERROR');
      }
    }
  }

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['display'] = array(
      '#type' => 'fieldset',
      '#title' => 'Format Output',
      '#description' => t('Alter the way a sequence is displayed')
    );

    $form['display']['num_bases_per_line'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of bases per line'),
      '#description' => t('Specify the number of bases per line. An HTML <br> tag ' .
        'will be inserted after the number of bases indicated. If no value is ' .
        'provided. The sequence will be one long string (default)'),
      '#default_value' => $this->options['display']['num_bases_per_line'],
    );
    $form['display']['derive_from_parent'] = array(
      '#type' => 'checkbox',
      '#title' => t('Derive sequence from parent'),
      '#description' => t('Rather than use the sequence from the \'residues\' of this feature, you may ' .
        'derive the sequence from the parent features to which it is aligned. This is useful in the case that the feature ' .
        'does not have sequence associated with it and we need to get it through it\'s alignment. ' . 
        'Note: this will slow queries with large numbers of results on the page.'),
      '#default_value' => $this->options['display']['derive_from_parent'],
    );
    $form['display']['aggregate'] = array(
      '#type' => 'checkbox',
      '#title' => t('Aggregate sub features'),
      '#description' => t('If the feature has sub features (e.g. CDS of an mRNA) then check this '.
        'box to filter the sequence to only include the sub features.  Gaps between sub features will be '.
        'excluded from the sequence.  This is useful for obtaining a complete CDS from an mRNA '.
        'without intronic sequence'),
      '#default_value' => $this->options['display']['aggregate'],
    );
  }

  function query() {
    parent::query();
    
    // if we are going to get the sequence from the parent then
    // we will need to do more queries in the render function
    // and we must have the feature_id to do those
    if($this->options['display']['derive_from_parent']){
      $this->ensure_my_table();
      $this->query->add_field($this->table,'feature_id');
      $this->query->add_field($this->table,'name');
    }
  }
    
  /**
  * Prior to display of results we want to format the sequence
  */
  function render($values) {
    $residues = '';
      
    // get the number of bases to show per line
    $num_bases_per_line = $this->options['display']['num_bases_per_line'];

    // get the residues from the feature.residues column
    $field = $this->field_alias;
    
    // get the feature id
    $feature_id = $values->feature_feature_id;
    $feature_name = $values->feature_name;
    
    // the upstream and downstream values get set by the 
    // tripal_views_handlers_filter_sequence.inc
    $upstream = $this->view->sequence_q['upstream'];
    $downstream = $this->view->sequence_q['downstream'];           
    if (!$upstream) {
       $upstream = 0;
    }
    if (!$downstream) {
       $downstream = 0;
    }
    
    // if we need to get the sequence from the parent but there is no aggregation
    // then do so now.
    if ($this->options['display']['derive_from_parent']) {             
      
      // execute our prepared statement
      if (tripal_core_is_sql_prepared('sequence_by_parent')) {
        $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";
        $parents = chado_query($sql, $upstream, $downstream, $feature_id);
      }

      while ($parent = db_fetch_object($parents)) {  
        $seq = '';  // initialize the sequence for each parent
                
        // if we are to aggregate then we will ignore the feature returned
        // by the query above and rebuild it using the sub features
        if ($this->options['display']['aggregate']){
          
          // now get the sub features that are located on the parent.
          $sql = "EXECUTE sub_features (%d, %d)";
          $children = chado_query($sql, $feature_id, $parent->srcfeature_id);
          $sql = "EXECUTE count_sub_features (%d, %d)";
          $num_children = db_fetch_object(chado_query($sql, $feature_id, $parent->srcfeature_id));
                 
          // iterate through the sub features and concat their sequences. They
          // should already be in order.
          $types = '';
          $i = 0;
          while($child = db_fetch_object($children)) {
            // keep up with the types
            if(!in_array($child->type_name,$types)){
              $types[] = $child->type_name;
            }
            
            $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";

            // if the first sub feature we need to include the upstream bases
            if ($i == 0 and $parent->strand >= 0) {
              // -------------------------- ref
              //    ....---->  ---->        
              //     up    1       2         
              $q = chado_query($sql, $upstream, 0, $child->feature_id);
            }
            elseif ($i == 0 and $parent->strand < 0) {
              // -------------------------- ref
              //    ....<----  <----
              //    down  1       2
              $q = chado_query($sql, 0, $downstream, $child->feature_id);
            }          
            // if the last sub feature we need to include the downstream bases
            elseif ($i == $num_children->num_children - 1 and $parent->strand >= 0) {
              // -------------------------- ref
              //        ---->  ---->....
              //          1       2 down
              $q = chado_query($sql, 0, $downstream, $child->feature_id);
            }
            elseif ($i == $num_children->num_children - 1 and $parent->strand < 0) {
              // -------------------------- ref
              //        <----  <----....
              //          1       2  up
              $q = chado_query($sql, $upstream, 0, $child->feature_id);
            }
            
            // for internal sub features we don't want upstream or downstream bases
            else {         
              $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";
              $q = chado_query($sql, 0, 0, $child->feature_id);
            }
            
            while($subseq = db_fetch_object($q)){
              // concatenate the sequences of all the sub features            
              if($subseq->srcfeature_id == $parent->srcfeature_id){
                $seq .= $subseq->residues;   
              }
            }                 
            $i++;
          } 
        } 
        // if this isn't an aggregate then use the parent residues
        else {
           $seq = $parent->residues;
        }
                              
        // get the reverse compliment if feature is on the reverse strand
        $dir = 'forward';
        if ($parent->strand < 0) {
          $seq = trpial_feature_reverse_complement($seq);
          $dir = 'reverse';
        }
        
        // now format for display
        $seq = wordwrap($seq, $num_bases_per_line, "<br>", TRUE);
        $residues .= ">$feature_name ($parent->typename) $parent->srcname:" . ($parent->adjfmin + 1) . ".." . $parent->adjfmax ." ($dir). ";
        if (count($types) > 0) {
          $residues .= "Excludes all bases but those of type(s): " . implode(', ',$types) . ". " ;
        }
        if ($parent->upstream > 0) {
           $residues .= "Includes " . $parent->upstream . " bases upstream.  ";
        }
        if ($parent->downstream > 0) {
           $residues .= "Includes " . $parent->downstream . " bases downstream.  ";
        }
        if (!$seq) {
          $residues .= "<br>\nNo sequence available\n<br>";          
        }
        else {
          $residues .= "<br>\n" . $seq . "\n<br>";          
        }
      }
    }
    // if we are not getting the sequence from the parent sequence then
    // use what comes through from the feature record
    else {
      $residues = $values->$field; 
      $residues = wordwrap($residues, $num_bases_per_line, "<br>", TRUE);  
    }
    
    // format the residues for display
    if($residues and $num_bases_per_line){
      $residues = '<span style="font-family: monospace;">' . $residues . '</span>';
    }
    return $residues;         
  } 
}
