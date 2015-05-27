Drupal.behaviors.tripalFeature_adminSummaryChart = {
  attach: function (context, settings) {

    // First add the container after the view header.
    var container = d3.select('#tripal-feature-admin-summary');
    if (container.empty) {
      container = d3.select('.view-header').append('div')
        .attr('id', 'tripal-feature-admin-summary')
        .classed('tripal-admin-summary',true);
    }

    // Set-up the dimensions for our chart canvas.
    var margin = {top: 20, right: 20, bottom: 100, left: 100},
        width = 960 - margin.left - margin.right,
        height = 500 - margin.top - margin.bottom;

    var color = d3.scale.ordinal()
        .range(["#a6cee3","#1f78b4","#b2df8a","#33a02c","#fb9a99","#e31a1c","#fdbf6f","#ff7f00","#cab2d6","#6a3d9a","#ffff99","#b15928"]);

    var formatNum = d3.format("0,000");

    // Set-up the scales of the chart.
    var x0 = d3.scale.ordinal()
        .rangeRoundBands([0, width], .1);
    var x1 = d3.scale.ordinal();
    var y = d3.scale.linear()
        .range([height, 0]);

    // Now set-up the axis functions.
    var xAxis = d3.svg.axis()
        .scale(x0)
        .orient('bottom');
    var yAxis = d3.svg.axis()
        .scale(y)
        .orient('left')
        .ticks(10, '');

    // Create our chart canvas.
    var svg = d3.select('#tripal-feature-admin-summary').append('svg')
        .attr('width', width + margin.left + margin.right)
        .attr('height', height + margin.top + margin.bottom)
      .append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');

    // The data was parsed and saved into tripalFeature.admin.summary
    // in the preprocess function for this template.
    if (Drupal.settings.tripalFeature.admin.summary) {

      // map the data to the x & y axis' of our chart.
      data = Drupal.settings.tripalFeature.admin.summary;
      x0.domain(data.map(function(d) { return d.name; }));
      x1.domain(Drupal.settings.tripalFeature.admin.organisms).rangeRoundBands([0, x0.rangeBand()]);
      //y.domain([0, d3.max(data, function(d) { return d3.max(d.organisms, function(d) { return d.value; }); })]);
      y.domain([0, d3.max(data, function(d) { return d.total_features; })]);

      // Create the x-axis.
      var xaxis = svg.append('g')
          .attr('class', 'x axis')
          .attr('transform', 'translate(0,' + height + ')')
          .call(xAxis);

      // Wrap the scientific names so they fit better.
      xaxis.selectAll(".tick text")
          .call(wrap, x0.rangeBand());

      // Label the  x-axis.
      xaxis.append('g')
        .attr('class', 'axis-label')
          .attr('transform', 'translate(' + width/2 + ',60)')
        .append('text')
          .attr('font-size', '16px')
          .attr('dy', '.71em')
          .style('text-anchor', 'middle')
          .text('Types of Features');

      // Create the y-axis.
      var yaxis = svg.append('g')
          .attr('class', 'y axis')
          .call(yAxis);

      // Label the y-axis.
      yaxis.append('g')
        .attr('class', 'axis-label')
          .attr('transform', 'translate(-70,' + height/2 + ')')
        .append('text')
          .attr('transform', 'rotate(-90)')
          .attr('font-size', '16px')
          .attr('dy', '.71em')
          .style('text-anchor', 'middle')
          .text('Total Number of Features');

      // Add a g element to contain each set of bars (1 per type).
      var type = svg.selectAll(".type")
          .data(data)
        .enter().append("g")
          .attr("class", "g")
          .attr("transform", function(d) { return "translate(" + x0(d.name) + ",0)"; });

      // Now add the bars :)
      // Keep in mind some processing was done in the preprocess function to
      // generate the bars array based on the organisms array
      // and pre-calculated the y0 & y1 used here.
      type.selectAll("rect")
          .data(function(d) { return d.bars; })
        .enter().append("rect")
          .attr("width", x0.rangeBand())
          .attr("y", function(d) { return y(d.y1); })
          .attr("height", function(d) { return y(d.y0) - y(d.y1); })
          .style("fill", function(d) { return color(d.name); })
        .append("svg:title")
          .text(function(d) { return formatNum(d.y1 - d.y0); });

      // Add the total to the top of the bar.
      svg.selectAll("text.bar")
        .data(data)
      .enter().append("text")
        .attr("class", "bar-label")
        .attr("text-anchor", "middle")
        .attr("x", function(d) { return x0(d.name) + x0.rangeBand()/2; })
        .attr("y", function(d) { return y(d.total_features) -5; })
        .text(function(d) { return formatNum(d.total_features); });

      // Finally add in a simple legend.
      var legend = svg.selectAll(".legend")
          .data(Drupal.settings.tripalFeature.admin.organisms.slice().reverse())
        .enter().append("g")
          .attr("class", "legend")
          .attr("transform", function(d, i) { return "translate(0," + i * 20 + ")"; });
      legend.append("rect")
          .attr("x", width - 18)
          .attr("width", 18)
          .attr("height", 18)
          .style("fill", color);
      legend.append("text")
          .attr("x", width - 24)
          .attr("y", 9)
          .attr("dy", ".35em")
          .style("text-anchor", "end")
          .attr('font-style','italic')
          .text(function(d) { return d; });

      function wrap(text, width) {
        text.each(function() {
          var text = d3.select(this),
              words = text.text().split(/[\s_]+/).reverse(),
              word,
              lineNumber = 0,
              lineHeight = 1.1, // ems
              y = text.attr("y"),
              dy = parseFloat(text.attr("dy")),
              tspan = text.text(null).append("tspan").attr("x", 0).attr("y", y).attr("dy", dy + "em");
          while (word = words.pop()) {
            tspan = text.append("tspan").attr("x", 0).attr("y", y).attr("dy", ++lineNumber * lineHeight + dy + "em").text(word);
          }
        });
      }
    }
  }
};
