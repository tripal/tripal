<?php
// Purpose: This template provides a generic layout for all Tripal nodes (page)

// get the template type of node (e.g. if type == chado_organism then template type == organism)
$node_type = $variables['type']; ?>

<script type="text/javascript">
(function ($) {
  Drupal.behaviors.<?php print $node_type?>Behavior = {
    attach: function (context, settings){ 
      // hide all but the first data block 
      $(".tripal-data-block").hide().filter(":first-child").show();
 
      // when a title in the table of contents is clicked, then 
      // show the corresponding item in the details box 
      $(".tripal_toc_list_item_link").click(function(){
        var id = $(this).attr('id') + "-tripal-data-block";
        $(".tripal-data-block").hide().filter("#"+ id).show();
        return false;
      }); 

      // if a ?block= is specified in the URL then we want to show the
      // requested block
      var block = window.location.href.match(/[\?|\&]block=(.+?)\&/)
      if(block == null){
        block = window.location.href.match(/[\?|\&]block=(.+)/)
      }
      if(block != null){
        $(".tripal-data-block").hide().filter("#" + block[1] + "-tripal-data-block").show();
      }
    }
  };
})(jQuery);
</script>

<div id="tripal_<?php print $node_type?>_content" class="tripal-contents"> 
  <table id="tripal-contents-table">
    <tr class="tripal-contents-table-tr">
      <td nowrap class="tripal-contents-table-td tripal-contents-table-td-toc"  align="left"><?php
        print $content['tripal_toc']['#value'] ?>
          
          <!-- Resource Links CCK elements --><?php
          if(property_exists($node, 'field_resource_links')) {
            for($i = 0; $i < count($node->field_resource_links); $i++){
              if($node->field_resource_links[$i]['value']){
                $matches = preg_split("/\|/",$node->field_resource_links[$i]['value']);?>
                <li><a href="<?php print $matches[1] ?>" target="_blank"><?php print $matches[0] ?></a></li><?php
              }
            }
          } ?> 
      </td>
      <td class="tripal-contents-table-td-data" align="left" width="100%">
         <!-- Resource Blocks CCK elements --> <?php
         if (property_exists($node, 'field_resource_titles')) {
           for ($i = 0; $i < count($node->field_resource_titles); $i++){
             if ($node->field_resource_titles[$i]['value']){ ?>
               <div id="tripal_<?php print $node_type?>-resource_<?php print $i?>-box" class="tripal_<?php print $node_type?>-info-box tripal-info-box">
                 <div class="tripal_<?php print $node_type?>-info-box-title tripal-info-box-title"><?php print $node->field_resource_titles[$i]['value'] ?></div>
                 <?php print $node->field_resource_blocks[$i]['value']; ?>
               </div> <?php
             }
           } 
         }?>
         <!-- Let modules add more content -->
         <?php
           foreach ($content as $key => $values) {
             if (array_key_exists('#value', $values) and $key != 'tripal_toc') {
               print $content[$key]['#value'];
             }
           }
         ?>
      </td>
    </tr>
  </table>
</div> 


