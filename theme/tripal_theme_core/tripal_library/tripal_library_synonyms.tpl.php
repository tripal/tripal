<?php
$library = $variables['node']->library;

// expand the library object to include the synonyms from the library_synonym 
// table in chado.
$library = tripal_core_expand_chado_vars($library,'table','library_synonym');

// get the references. if only one reference exists then we want to convert
// the object into an array, otherwise the value is an array
$synonyms = $library->library_synonym;
if (!$synonyms) {
   $synonyms = array();
} elseif (!is_array($synonyms)) { 
   $synonyms = array($synonyms); 
}

?>
<div id="tripal_library-synonyms-box" class="tripal_library-info-box tripal-info-box">
  <div class="tripal_library-info-box-title tripal-info-box-title">Synonyms</div>
  <div class="tripal_library-info-box-desc tripal-info-box-desc">The library '<?php print $library->name ?>' has the following synonyms</div>
  <?php if(count($synonyms) > 0){ ?>
  <table id="tripal_library-synonyms-table" class="tripal_library-table tripal-table tripal-table-horz">
    <tr>
      <th>Synonym</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($synonyms as $library_synonym){
      $class = 'tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php print $library_synonym->synonym_id->name?></td>
      </tr>
      <?php
      $i++;  
    } ?>
  </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no synonyms for this library</div> 
  <?php }?>
</div>
