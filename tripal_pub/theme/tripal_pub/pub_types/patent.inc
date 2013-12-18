<table id="tripal_pub-table-base" class="tripal_pub-table tripal-table tripal-table-vert">
  <tr class="tripal_pub-table-even-row tripal-table-even-row">
    <th>Title</th>
    <td><?php
      if ($url) {
        print l(htmlspecialchars($pub->title), $url, array('attributes' => array('target' => '_blank')));          
      }
      elseif ($dbxref->db_id->urlprefix) { 
        print l(htmlspecialchars($pub->title), $dbxref->db_id->urlprefix . $dbxref->accession, array('attributes' => array('target' => '_blank')));             
      } 
      else {
        print htmlspecialchars($pub->title); 
      }?>
    </td>
  </tr>
  <tr class="tripal_pub-table-odd-row tripal-table-odd-row">
    <th>Authors</th>
    <td><?php print $authors->value ? $authors->value : 'N/A'; ?></td>
  </tr>
  <tr class="tripal_pub-table-even-row tripal-table-even-row">
    <th>Type</th>
    <td><?php print $pub->type_id->name; ?></td>
  </tr>
  <tr class="tripal_pub-table-odd-row tripal-table-odd-row">    
    <th>Year</th>
    <td><?php print $pub->pyear; ?></td>
  </tr>
  <tr class="tripal_pub-table-even-row tripal-table-even-row">
    <th>Citation</th>
    <td><?php print htmlspecialchars($citation->value); ?></td>
  </tr>
  <tr class="tripal_pub-table-odd-row tripal-table-odd-row">
    <th>Abstract</th>
    <td style="text-align:justify;"><?php print htmlspecialchars($abstract->value) ? $abstract->value : 'N/A'; ?></td>
  </tr>
</table>