/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/ModuleUtils.h"
#include "nsIModule.h"

#include "nsINodeLoader.h"
#include "NodeLoader.h"
#include "nsString.h"
#include "nsComponentManagerUtils.h"
#include "NodeProcessParent.h"
#include "NodeParent.h"
#include "NodeBindings.h"
#include "nsAppRunner.h"
#include "nsDirectoryService.h"
#include "nsDirectoryServiceDefs.h"

using namespace mozilla;

////////////////////////////////////////////////////////////////////////
// Define the contructor function for the objects
//
// NOTE: This creates an instance of objects by using the default constructor
//
NS_GENERIC_FACTORY_CONSTRUCTOR(NodeLoader)

////////////////////////////////////////////////////////////////////////
// Define a table of CIDs implemented by this module along with other
// information like the function to create an instance, contractid, and
// class name.
//
#define NS_NODELOADER_CONTRACTID \
  "@mozilla.org/spiderweb/nodeloader;1"

#define NS_NODELOADER_CID             \
{ /* 019718E3-CDB5-11d2-8D3C-000000000000 */    \
0x019618e3, 0xcdb5, 0x11d2,                     \
{ 0x8d, 0x3c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } }

/* Implementation file */
NS_IMPL_ISUPPORTS(NodeLoader, nsINodeLoader)

NodeLoader::NodeLoader()
{
  /* member initializers and constructor code */
}

NodeLoader::~NodeLoader()
{
  // if (nodeParent) {
  //   nodeParent->DeleteProcess();
  //   delete nodeParent;
  // }
}

/* void start (); */
NS_IMETHODIMP NodeLoader::Start(const nsACString & script, nsINodeObserver *observer, JSContext* cx)
{
  if (mNodeBindings) {
    printf(">>> Node already started!\n");
    return NS_OK;
  }
  // nodeParent = new node::NodeParent(script, observer);
  // if (NS_FAILED(nodeParent->LaunchProcess())) {
  //   delete nodeParent;
  //   return NS_ERROR_FAILURE;
  // }
  nsAutoCString autoScript(script);

  // Build the path to the init script.
  nsCOMPtr<nsIFile> greDir;
  nsDirectoryService::gService->Get(NS_GRE_DIR,
                                    NS_GET_IID(nsIFile),
                                    getter_AddRefs(greDir));
  MOZ_ASSERT(greDir);
  greDir->AppendNative(NS_LITERAL_CSTRING("modules"));
  greDir->AppendNative(NS_LITERAL_CSTRING("spiderweb"));
  greDir->AppendNative(NS_LITERAL_CSTRING("init.js"));
  nsAutoString initScript;
  greDir->GetPath(initScript);

  JSObject* globalObject = JS::CurrentGlobalOrNull(cx);
  MOZ_ASSERT(globalObject);
  mNodeBindings = NodeBindings::Create();
  int argc = 3;
  char **argv = new char *[argc + 1];
  argv[0] = gArgv[0];
  argv[1] = const_cast<char*>(ToNewCString(initScript));
  argv[2] = const_cast<char*>(autoScript.get());
  // NodeBindings* nb = NodeBindings::Instance();
  // nb->Initialize(this, aInitArgs.Length(), args);
  mNodeBindings->Initialize(cx, globalObject, argc, argv);

  return NS_OK;
}

NS_IMETHODIMP NodeLoader::PostMessage(const nsACString & msg, JSContext* cx)
{
  // nsAutoCString autoMsg(msg);
  // if (!nodeParent->SendMessage(autoMsg)) {
  //   return NS_ERROR_FAILURE;
  // }
  return NS_OK;
}

NS_DEFINE_NAMED_CID(NS_NODELOADER_CID);

static const mozilla::Module::CIDEntry kEmbeddingCIDs[] = {
    { &kNS_NODELOADER_CID, false, nullptr, NodeLoaderConstructor },
    { nullptr }
};

static const mozilla::Module::ContractIDEntry kEmbeddingContracts[] = {
    { NS_NODELOADER_CONTRACTID, &kNS_NODELOADER_CID },
    { nullptr }
};

static const mozilla::Module kEmbeddingModule = {
    mozilla::Module::kVersion,
    kEmbeddingCIDs,
    kEmbeddingContracts
};

NSMODULE_DEFN(NodeLoader) = &kEmbeddingModule;
