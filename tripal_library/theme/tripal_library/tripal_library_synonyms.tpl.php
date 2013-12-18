<?php
$library = $variables['node']->library;

// expand the library object to include the synonyms from the library_synonym 
// table in chado.
$options = array('return_array' => 1); 
$library = tripal_core_expand_chado_vars($library,'table','library_synonym', $options);
$synonyms = $library->library_synonym;

if (count($synonyms) > 0) { ?>
  <div id="tripal_library-synonyms-box" class="tripal_library-info-box tripal-info-box">
    <div class="tripal_library-info-box-title tripal-info-box-title">Synonyms</div>
    <div class="tripal_library-info-box-desc tripal-info-box-desc">The library '<?php print $library->name ?>' has the following synonyms</div>
    <table id="tripal_library-synonyms-table" class="tripal_library-table tripal-table tripal-table-horz">
      <tr>
        <th>Synonym</th>
      </tr> <?php
      $i = 0; 
      foreach ($synonyms as $library_synonym) {
        $class = 'tripal-table-odd-row';
        if ($i % 2 == 0 ) {
          $class = 'tripal-table-even-row';
        }?>
        <tr class="<?php print $class ?>">
          <td> <?php print $library_synonym->synonym_id->name?></td>
        </tr> <?php
        $i++;  
      } ?>
    </table>
  </div> <?php 
}
