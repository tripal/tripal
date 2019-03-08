<?php
/**
 *
 */
$biological_views = module_invoke_all('search_biological_data_views');
$disabled_views = variable_get('views_defaults', []);
?>

<p>Use the following to search the various types of biological content available
    by
    this site:</p>
<DL>
  <?php foreach ($biological_views as $view) { ?>
    <?php if (!isset($disabled_views[$view['machine_name']]) OR ($disabled_views[$view['machine_name']] == FALSE)) { ?>
          <DT><?php print l($view['human_name'], $view['link']); ?></DT>
          <DD><?php print $view['description']; ?></DD>
    <?php }
  } ?>
</DL>

<?php
// How to disable/remove views from this list
print tripal_set_message(
  "To remove a view from this list, simply navigate to the Views UI (Administer ->
    Structure -> Views; admin/structure/views) and choose 'disable' from the action
    drop-down to the right of the view you would like to remove.",
  TRIPAL_INFO,
  ['return_html' => 1]
);

// Tell Tripal admin how to add views to this list
print tripal_set_message(
  "Developers: To add a view or other search tool to the above list you need to 
    implement hook_search_biological_data_views() in your custom module.
    This hook should return an array as follows: <code><pre>array(
    '[view-machine-name]' => array(
      'machine_name' => '[view-machine-name]',
      'human_name' => '[Human-readable title to show in above list]',
      'description' => '[description to show in above list]',
      'link' => '[path to the view]'
    ),
  );</pre></code>
  Where you should replace all instructions in square-brackets([]) with the details of your view.",
  TRIPAL_INFO,
  ['return_html' => 1]
);

// Tell Tripal Admin which template to change
print tripal_set_message(
  "Administrators, you can customize the way the content above is presented.  Tripal
    provides a template file for each block of content.  To customize, copy the template
    file to your site's default theme, edit then " .
  l('clear the Drupal cache', 'admin/config/development/performance', ['attributes' => ['target' => '_blank']]) .
  ". Currently, the content above is provided by this template: <br><br>$template_file",
  TRIPAL_INFO,
  ['return_html' => 1]
);
?>
