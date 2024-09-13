import unreal

def listAssetPaths():
    
    EAL = unreal.EditorAssetLibrary
    
    assetPaths = EAL.list_assets('/Game')
    
    for assetPath in assetPaths: print(assetPath)
    
def getSelectionContentBrowser():
    
    EUL = unreal.EditorUtilityLibrary
    
    selectedAssets = EUL.get_selected_assets()
    
    for selectedAsset in selectedAssets: print(selectedAsset)
    
def getAllActors():

    EAS = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    
    actors = EAS.get_all_level_actors()
    
    for actor in actors: print(actor)
    
 
def getSelectedActors():
    
    EAS = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    
    actors = EAS.get_selected_level_actors()
    
    for actor in actors: print(actor)
    
def getAssetClass(classType):
    EAL = unreal.EditorAssetLibrary
    assetPaths = EAL.list_assets('/Game')
    
    assets = []
    
    for assetPath in assetPaths:
        assetData = EAL.find_asset_data(assetPath)
        assetClass = assetData.asset_class
        print(assetClass)
        if assetClass == classType:
            assets.append(assetData.get_asset())
    
    for asset in assets : print(asset)
    return assets
    
def getStaticMeshData():
    staticMeshes = getAssetClass('StaticMesh')
    for staticMesh in staticMeshes:
        assetImportData = staticMesh.get_editor_property('asset_import_data')
        print(assetImportData)