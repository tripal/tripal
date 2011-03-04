<!-- Add library information which this feature belongs to-->
           	<?php if ($node->lib_additions) { ?>
               <tr><th>Library</th><td>
                  <?php
                     $libraries = $node->lib_additions;
                     foreach ($libraries as $lib_url => $lib_name) {
                        // Check if library exists as a node in drupal
                        if ($lib_url) {
                  ?>
                     <a href="<?php print $lib_url?>"><?php print $lib_name?></a><BR>
                  <?php
                        } else {
                           print $lib_name;
                        }
                     }
                  ?>
               </td></tr>
            <?php } ?>
            <!-- End of library addition -->
