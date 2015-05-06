// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

chrome.tabs.onUpdated.addListener(function(tabId , changeInfo , tab){
  console.log("onUpdated :" + changeInfo.url);
});

chrome.tabs.onCreated.addListener(function(tab){
  console.log("onCreated");
});
