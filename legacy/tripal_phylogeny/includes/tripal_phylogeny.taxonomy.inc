<?php

/**
 * Generates a page that contains the taxonomy view.
 */
function tripal_phylogeny_taxonomy_view() {
  $values = [
    'type_id' => [
      'name' => 'taxonomy',
    ],
  ];

  $message = t('Site administrators:  This page is meant to provide
      a heirarchical taxonomic tree for all of the organism present
      in this site.  This may not be useful if you only have a few
      species. If so, you can turn off this page by disabling this page on
      the site\'s <a href="@menu">Navigation Menu</a>.  Otherwise, to generate the taxonomy go to this site\'s
      <a href="@taxloader">NCBI taxonomy loader</a> to import the taxonomy information from NCBI.
      <br><br>Note: If you add new species to this site, you should rerun the
        NCBI taxonomy loader to update the view</p>',
    [
      '@menu' => url('admin/structure/menu/manage/navigation'),
      '@taxloader' => url('admin/tripal/loaders/ncbi_taxonomy_loader'
      ),
    ]
  );
  $admin_message = tripal_set_message($message, TRIPAL_INFO, ['return_html' => TRUE]);

  $phylotree = chado_generate_var('phylotree', $values);
  if ($phylotree) {
    $node = new stdClass();
    $node->phylotree = $phylotree;

    $html = theme('tripal_phylogeny_taxonomic_tree', ['node' => $node]) .
      $admin_message;
    return $html;
  }

  return [
    '#type' => 'markup',
    '#markup' => t('This site has not yet prepared the taxonomy for viewing.') . $admin_message,
  ];
}
