/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Generator.cpp
 * \brief NS3 Code generate unit
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/Generator.h"

#include <boost/range/adaptor/indexed.hpp>

NetworkGenerator::NetworkGenerator(NetUnit net)
{
  // nodes
  this->nodes = net.GetNodes();
  // channels
  this->channels = net.GetChannels();
  // name
  this->name = net.GetName();
}

std::vector<std::string> NetworkGenerator::CppCode() {
  CodeSecretary lines;

  // namespace begin
  lines.push_back("namespace tgim {");
  // struct struct
  lines.push_back("struct " + name + " {");
  lines.indentRight();

  // declare
  gen_decl(lines);

  // build function
  gen_build(lines);

  lines.indentLeft();
  // struct end
  lines.push_back("};");
  // namespace end
  lines.push_back("}");

  return lines.get(); 
}

void NetworkGenerator::gen_decl(CodeSecretary& lines) {
  lines.push_back("/*******************************************************");
  lines.push_back(" * section [we can always use]                         *");
  lines.push_back(" ******************************************************/");

  // each node
  lines.push_back("// enum node name");
  lines.push_back("enum NodeName {");
  lines.indentRight();
  for (const auto& item : nodes) {
    const auto& node = item.second;
    lines.push_back("" + node.name + ",");
  }
  lines.indentLeft();
  lines.push_back("};");

  // %%
  lines.push_back("");
  
  // section before build
  lines.push_back("/*******************************************************");
  lines.push_back(" * section [config this when before build if you need] *");
  lines.push_back(" ******************************************************/");

  // channel helper
  lines.push_back("// helper");
  std::unordered_map<std::string, std::string> helper_name_map;
  helper_name_map["PointToPoint"] = "PointToPointHelper";
  helper_name_map["Csma"] = "CsmaHelper";
  for (const auto& item : channels) {
    const auto& ch_buf = item.second;
    if ( helper_name_map[ch_buf->GetType()] != "" ) {
      lines.push_back("// Channel : " + ch_buf->name );
      lines.push_back(helper_name_map[ch_buf->GetType()]
                        + " " + ch_buf->name + ";");
    } else {
      if ( ch_buf->nodes.size() < 2 )
      {
        lines.push_back("// [TGIM ERR] Channel '"
                          + ch_buf->name
                          + "' is node count is less then 2.");
      }
      else if ( ch_buf->nodes.size() == 2 )
      {
        lines.push_back("// [TGIM AUTO]");
        lines.push_back(helper_name_map["PointToPoint"]
                          + " " + ch_buf->name + ";");
      }
      else if ( ch_buf->nodes.size() > 2 )
      {
        lines.push_back("// [TGIM AUTO]");
        lines.push_back(helper_name_map["Csma"]
                          + " " + ch_buf->name + ";");
      }
    }
  }

  // %%
  lines.push_back("");
  
  // section after build
  lines.push_back("/*******************************************************");
  lines.push_back(" * section [when after build, we can use this]         *");
  lines.push_back(" ******************************************************/");
  // all nodes
  lines.push_back("// nodes");
  lines.push_back("NodeContainer " + name_all_nodes + ";");
  // netdevices
  for (const auto& item : channels) {
    const auto& channel = item.second;
    netdevs[channel->name] = "ndc_" + channel->name;
    lines.push_back("NetDeviceContainer " + netdevs[channel->name] + ";");
  }
}

void NetworkGenerator::gen_build(CodeSecretary& lines) {
  // function begin
  lines.push_back("");
  lines.push_back("/*******************************************************");
  lines.push_back(" * build function                                      *");
  lines.push_back(" ******************************************************/");
  lines.push_back("void " + this->name_build_func + "() {");
  lines.indentRight();

  // create all nodes
  lines.push_back("// create all nodes");
  lines.push_back(this->name_all_nodes
                + ".Create("
                + std::to_string(this->nodes.size())
                + ");");

  // name each node
  /*
  lines.push_back("// name each node");
  for (const auto& elem : nodes | boost::adaptors::indexed()) {
    const auto& node = elem.value().second;
    const auto& i = std::to_string(elem.index());
    lines.push_back(node.name + " = "
                  + this->name_all_nodes + ".Get(" + i + ");");
  }
  */

  //
  // connect nodes via link
  // 
  lines.push_back("// connect link");
  // -- connector generate
  std::unordered_map<std::string, std::function<void(Channel*)>> connector;
  connector["PointToPoint"]
    = [=, &lines](Channel *channel) { 
      Link link = *dynamic_cast<Link*>(channel);
      lines.push_back( netdevs[link.name] + " = "
                        + link.name + ".Install("
                        + this->name_all_nodes + ".Get(" + link.first + ")"
                        + ","
                        + this->name_all_nodes + ".Get(" + link.second + ")"
                      + ");" );
    };
  connector["Csma"]
    = [=, &lines](Channel *channel) { 
      Csma csma = *dynamic_cast<Csma*>(channel);
      // make node container
      lines.push_back( "{" );
      lines.indentRight();
      lines.push_back( "NodeContainer nc_local;" );
      // add node
      for (const auto& node : csma.nodes) {
        lines.push_back( "nc_local.Add("
                        + this->name_all_nodes + ".Get(" + node + ")"
                        + ");" );
      }
      // connect csma
      lines.push_back( netdevs[csma.name] + " = "
                        + csma.name + ".Install(nc_local);" );
      lines.indentLeft();
      lines.push_back( "}" );
    };
  // -- connect node to channel
  for (const auto& item : channels) {
    std::shared_ptr<Channel> ch_buf = item.second;
    if ( connector[ch_buf->GetType()] )
    {
      connector[ch_buf->GetType()](ch_buf.get());
    }
    else // auto set channel type, this will been generated latest for loop
    {
      // generate added channnel name
      std::string added_ch_name = "link_auto_"+std::to_string(channels.size());

      ch_buf.reset(new Channel(*ch_buf));  // replace to copy from original
      if ( ch_buf->nodes.size() < 2 )
      {
        lines.push_back("// [TGIM ERR] Channel '"
                          + ch_buf->name
                          + "' is node count is less then 2.");
      }
      else if ( ch_buf->nodes.size() == 2 )
      {
        Link *linkptr = new Link(*ch_buf);
        linkptr->first = ch_buf->nodes[0];
        linkptr->second = ch_buf->nodes[1];
        ch_buf.reset(static_cast<Channel*>(linkptr));
        auto_channels[ch_buf->name] = ch_buf;
      }
      else if ( ch_buf->nodes.size() > 2 )
      {
        Csma *csmaptr = new Csma(*ch_buf);
        ch_buf.reset(static_cast<Channel*>(csmaptr));
        auto_channels[ch_buf->name] = ch_buf;
      }
    }
  }
  for (const auto& item : auto_channels) {
    std::shared_ptr<Channel> ch_buf = item.second;
    if ( connector[ch_buf->GetType()] )
    {
      lines.push_back("// [TGIM AUTO]");
      connector[ch_buf->GetType()](ch_buf.get());
    }
  }

  //
  // install Internet stack
  //
  lines.push_back("// install internet stack");
  lines.push_back("InternetStackHelper stack;");
  for (const auto& item : nodes) {
    const auto& node = item.second;
    lines.push_back("stack.Install("
                      + this->name_all_nodes + ".Get(" + node.name + ")"
                      + ");" );
  }

  //
  // assign IP address
  //
  lines.push_back("// ");
  lines.push_back("NS_LOG_INFO (\"Assign ip addresses.\");");
  /* // assign ip addresses
   * NS_LOG_INFO ("Assign ip addresses.");
   * Ipv4AddressHelper ip;
   * ip.SetBase ("192.168.1.0", "255.255.255.0");
   * Ipv4InterfaceContainer addresses = ip.Assign (devs); */
  lines.push_back("{ Ipv4AddressHelper ip;");
  lines.indentRight();
  for (const auto& elem : netdevs | boost::adaptors::indexed()) {
    const auto& nd = elem.value().second;
    const auto& i = std::to_string(elem.index() + 1);
    lines.push_back("// " + nd);
    std::string base_ip = "192.168." + i + ".0";
    std::string base_subnet = "255.255.255.0";
    // set base
    lines.push_back( (std::string)"ip.SetBase ("
                      + "\"" + base_ip + "\""
                      + ","
                      + "\"" + base_subnet + "\""
                      + ");" );
    // ip assign
    lines.push_back ( "Ipv4InterfaceContainer addresses = ip.Assign (" + nd + ");" );
  }
  lines.indentLeft();
  lines.push_back("}");

  //
  // routing
  // 
  lines.push_back( "NS_LOG_INFO (\"Initialize Global Routing.\");" );
  lines.push_back( "Ipv4GlobalRoutingHelper::PopulateRoutingTables ();" );

  
  //
  // applications
  //

  
  /*
  lines.push_back( "// Applications" );
  // NS_LOG_INFO ("Create Source");
  lines.push_back( "Config::SetDefault (\"ns3::Ipv4RawSocketImpl::Protocol\", StringValue (\"2\"));" );
  lines.push_back( "InetSocketAddress dst = InetSocketAddress (addresses.GetAddress (3));" );
  lines.push_back( "OnOffHelper onoff = OnOffHelper (\"ns3::Ipv4RawSocketFactory\", dst);" );
  lines.push_back( "onoff.SetConstantRate (DataRate (15000));" );
  lines.push_back( "onoff.SetAttribute (\"PacketSize\", UintegerValue (1200));" );
  */

  /*

  ApplicationContainer apps = onoff.Install (c.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));

  NS_LOG_INFO ("Create Sink.");
  PacketSinkHelper sink = PacketSinkHelper ("ns3::Ipv4RawSocketFactory", dst);
  apps = sink.Install (c.Get (3));
  apps.Start (Seconds (0.0));
  apps.Stop (Seconds (11.0));

  NS_LOG_INFO ("Create pinger");
  V4PingHelper ping = V4PingHelper (addresses.GetAddress (2));
  NodeContainer pingers;
  pingers.Add (c.Get (0));
  pingers.Add (c.Get (1));
  pingers.Add (c.Get (3));
  apps = ping.Install (pingers);
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (5.0));
  AddressValue remoteAddress (InetSocketAddress (ifs3.GetAddress(1, 0), PORT));
  BulkSendHelper ftp ("ns3::TcpSocketFactory", Address());
  ftp.SetAttribute ("Remote", remoteAddress);
  ftp.SetAttribute ("MaxBytes",
     UintegerValue (int(DATA_MBYTES * 1024 * 1024)));

  ApplicationContainer sourceApp1 = ftp.Install (net1_nodes.Get(0));
    sourceApp1.Start (Seconds (SIM_START+0.1));
    sourceApp1.Stop (Seconds (SIM_STOP -0.1));

  Address sinkAddress (InetSocketAddress (Ipv4Address::GetAny (), PORT));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkAddress);

    ApplicationContainer sink_apps = sinkHelper.Install (net3_nodes.Get (1));
  sink_apps.Start (Seconds (SIM_START+0.1));
  sink_apps.Stop (Seconds (SIM_STOP -0.1));
  */

  /// function end
  lines.indentLeft();
  lines.push_back("}");
}
