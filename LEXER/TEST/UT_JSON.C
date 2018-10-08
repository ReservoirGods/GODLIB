#include	"..\JSON.H"

#include	<GODLIB\UNITTEST\UNITTEST.H>


const char * gpJsonTest = "{}";



GOD_UNIT_TEST( JSON )
{
#if 0
	sString jsonString;
	sNodeJSON * lpNode;

	JSONTest_NodeCount( "", 0 );
	JSONTest_NodeCount( "{}", 1 );
	JSONTest_NodeCount( "{ \"key\" }", 2 );
	JSONTest_NodeCount( "{ \"key\" : \"value\" }", 3 );

	String_Init( &jsonString, "{}" );
	lpNode = JSON_Parse( &jsonString );
	GOD_UNIT_TEST_EXPECT( ( 1 == JSON_GetNodeCount( lpNode )), "json node count mismatch" );
	JSON_DebugShow( lpNode );
	JSON_Destroy( lpNode );


/*
	String_Init( &jsonString, "{}" );
	lpNode = JSON_Parse( &jsonString );
	JSON_Destroy( lpNode );



	String_Init( &jsonString, "{ \"value1\" : 12 }" );
	lpNode = JSON_Parse( &jsonString );
	JSON_Destroy( lpNode );
	String_DeInit( &jsonString );
*/
//	String_Init( &jsonString, "{ \"value1\" : 12, \"value2\" : \"forty\", \"array\" : [ 6, 7, 9 ]  }" );
	String_Init( &jsonString, "{ \"object1\" : { \"field0\" : \"value0\" }, \"object2\" : { \"field2\" : \"good\" } }" );
//	String_Init( &jsonString, "{ \"objectArray\" : [ { \"field0\" : \"value0\" }, { \"field1\" : \"value1\" } ] }" );
	lpNode = JSON_Parse( &jsonString );
	JSON_DebugShow( lpNode );
	JSON_Destroy( lpNode );
	String_DeInit( &jsonString );
#endif
}