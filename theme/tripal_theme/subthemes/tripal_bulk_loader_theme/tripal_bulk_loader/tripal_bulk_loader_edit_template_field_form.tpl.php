
<?php print $form['edit_fields']['#prefix']; ?>
<fieldset><legend> <?php print $form['edit_fields']['#title']; ?> </legend>
<?php
  print drupal_render($form['template_name']);

  // Foreach element in the form fieldset 'edit_fields'
  foreach ($form['edit_fields'] as $key => $form_array) {
    if (preg_match('/^#/',$key)) { continue; }
    
    // We only care about the additional fieldset
    if (preg_match('/additional/', $key)) {
      //print fieldset
      if ($form_array['#collapsible']) { $class[] = 'collapsible'; }
      if ($form_array['#collapsed']) { $class[] = 'collapsed'; }
      if (sizeof($class)) { $class = ' class="'.implode(' ',$class).'"'; }
      print '<fieldset'.$class.'><legend>'.$form_array['#title'].'</legend>';
      
      // Foreach element in the 'additional' fieldset
      foreach ($form_array as $key => $sub_form_array) {
        if (preg_match('/^#/',$key)) { continue; }
        
        // We only care about the 'regex_transform' fieldset
        if (preg_match('/regex_transform/', $key)) {
          
          // print fieldset
          if ($sub_form_array['#collapsible']) { $class[] = 'collapsible'; }
          if ($sub_form_array['#collapsed']) { $class[] = 'collapsed'; }
          if (sizeof($class)) { $class = ' class="'.implode(' ',$class).'"'; }
          print '<fieldset'.$class.'><legend>'.$sub_form_array['#title'].'</legend>';
          
          // print description
          print drupal_render($sub_form_array['regex_description']);
          
          // Render Draggable Table
          drupal_add_tabledrag('draggable-table', 'order', 'sibling', 'transform-reorder');
          $header = array('Match Pattern', 'Replacement Pattern', 'Order', '');
          $rows = array();
          foreach ($sub_form_array['regex-data'] as $key => $element) {
            if (preg_match('/^#/',$key)) { continue; }
            $element['new_index']['#attributes']['class'] = 'transform-reorder';
            
            $row = array();
            $row[] = drupal_render($element['pattern']);
            $row[] = drupal_render($element['replace']);
            $row[] = drupal_render($element['new_index']) . drupal_render($element['id']);
            $row[] = drupal_render($element['submit-delete']);
            $rows[] = array('data' => $row, 'class' => 'draggable');
          }
          
          print theme('table', $header, $rows, array('id' => 'draggable-table'));          
          
          // render remaining elements
          foreach ($sub_form_array as $key => $s2_form_array) {
            if (preg_match('/^#/',$key)) { continue; }
            if (!preg_match('/regex-data/', $key)) {
              print drupal_render($s2_form_array);
            }
          }
          
          print '</fieldset>';
          
        } else {
          // render other elements  in additional fieldset
          print drupal_render($sub_form_array);
        }
      }
      print '</fieldset>';
    } else {
      // render other elements in edit_fields fieldset
      print drupal_render($form_array);
    }
  }
  unset($form['edit_fields']);
?>

</fieldset>
</div>

<?php
  //Render remaining -Needed to submit
  print drupal_render($form);
?>