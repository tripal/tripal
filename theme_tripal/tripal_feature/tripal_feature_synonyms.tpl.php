<!-- theme_tripal_feature_feature_synonyms -->
            <?php
               $synonyms = $node->synonyms;
               if(count($synonyms) > 0){
            ?>
      			<tr><th>Synonyms</th><td>
                  <?php
                  // iterate through each synonym
                  if (is_array($synonyms)) {
                     foreach ($synonyms as $result){
                        print $result->name."<br>";
                     }
                  } else {
                     print $synonyms;
                  }
                  ?>
               	</td></tr>
            <?php } ?>
      		<!-- End of theme_tripal_feature_feature_synonyms -->
