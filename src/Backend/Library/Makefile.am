noinst_LIBRARIES = libifx.a

libifx_a_SOURCES = AddStamp.c AddTimeStamp.c AddToEquivCla.c AddToGraphLis.c AddToNodeList.c AdeAlloc.c ArchiveStuff.c AreConstsEqua.c AreEdgesEqual.c AreNodesDepen.c AreNodesEqual.c AreValuesEqua.c AssignNewKpor.c AssignNewLabe.c AssignNewLoop.c AssignNewLpor.c AssignNewMpor.c AssignNewRpor.c AssignNewTpor.c AssignSourceF.c AssignTags.c AssocListAllo.c AttachEdge.c BuildItems.c CallAlloc.c CallItems.c CaseCmp.c CanPropagateC.c ChangeEdgeToC.c ChangeExportP.c ChangeExports.c ChangeImportP.c ChangeToConst.c CheckEdge.c ClearTouchFla.c CopyEdge.c CopyEdgeAndLi.c CopyEdgeAndTh.c CopyString.c Count.c CountItems.c CreateAndInse.c CreateNewEqui.c DirectEdge.c DontCombineFi.c EdgeAlloc.c ElapsedTime.c EnterInSet.c EnterScope.c Error1.c Error2.c ExitScope.c FastAreNodesE.c FindEnclosing.c FindExport.c FindFunction.c FindGraph.c FindGraphNode.c FindImport.c FindInfo.c FindLabel.c FindLargestEx.c FindLargestIm.c FindLargestLa.c FindLargestPo.c FindLastGraph.c FindLastImpor.c FindLastNode.c FindNode.c FindReturns.c FindSource.c FixConstantIm.c FixSignedCons.c FoldItems.c GatherOthers.c GetAggregateT.c GetNewCallNod.c GetSourceLine.c GetStampStrin.c HasWriteExpor.c CopyEdgeAndRe.c CopyExports.c CopyImports.c CopyNode.c If1Read.c If1Write.c If2Read.c If2Write.c ImportSwap.c InfoAlloc.c InitEquivClas.c IntToAscii.c IntegerPower.c IsABRecord.c IsAdePresent.c IsAggregate.c IsCInterface.c IsCalled.c IsCaller.c IsCandidate.c IsDivByZero.c IsEntryPoint.c IsFortranInte.c IsInnerLoop.c IsIntrinsic.c IsPMarked.c IsPath.c IsPathInit.c IsReadOnly.c IsReductionIn.c IsStamp.c LinkAdeExport.c LinkAdeImport.c LinkAssocList.c LinkExport.c LinkExportLis.c LinkExportToE.c LinkGraph.c LinkImport.c LinkImportLis.c LinkNode.c LinkNodeLists.c LookupInfo.c LowerCase.c MakeAde.c MakeCompound.c MakeConst.c MakeEdge.c MakeGraph.c MakeInfo.c MakeNode.c MakePtrTypes.c MonoIf1Read.c MonoIf1Write.c MonoMakeCompo.c MonoMakeConst.c MonoMakeEdge.c MonoMakeGraph.c MonoMakeNode.c MonoReadAssoc.c MonoWriteCons.c MonoWriteInfo.c MonoWriteNode.c MyAlloc.c MyBBlockAlloc.c NameAlloc.c NewType.c NodeAlloc.c NormalizeNode.c PPrintConst.c PPrintIndenta.c Patches.c PlaceGraph.c PlaceNode.c PointToHead.c PopGraphList.c PopNodeList.c PrepareForNex.c PrintLInfo.c PrintWarning.c PropagateCons.c PushGraphList.c PushNodeList.c ReadAssocList.c ReadError.c ReadInteger.c ReadItems.c ReadLineId.c ReadLiteral.c ReadPragmas.c ReadStamp.c ReadString.c RemoveFromEqu.c RemoveStamp.c SameEquivClas.c SetAlloc.c SmashItems.c StartProfiler.c Stop.c StopProfiler.c TSECND.c TempAlloc.c ThreadToUse.c TimeItems.c TouchTheUnion.c UnlinkAdeExpo.c UnlinkAdeImpo.c UnlinkExport.c UnlinkGraph.c UnlinkImport.c UnlinkNode.c UpperCase.c UsageCount.c Warning1.c WriteConst.c WriteGlobals.c WriteImports.c WriteInfo.c WriteLoopMap.c WriteNodes.c WriteStamps.c WriteTheLMap.c world.h

nodist_libifx_a_SOURCES = nametabl.c

noinst_HEADERS = IFX.h

noinst_PROGRAMS = makenames

makenames_SOURCES = makenames.c world.h

CLEANFILES = nametabl.c

nametabl.c: IFX.h makenames$(EXEEXT)
	$(RM) nametabl.c
	./makenames < '$(srcdir)/IFX.h' > nametabl.c
