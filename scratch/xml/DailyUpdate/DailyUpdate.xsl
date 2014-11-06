<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="text" encoding="UTF8"/>
  <xsl:template match="DailyUpdate/class" mode="h">
---------------------------<xsl:value-of select="@name"/>.h---------------------------
#import "Uploadable.h"
@interface <xsl:value-of select="@name"/>:<xsl:value-of select="base_class/@name"/>
    <xsl:apply-templates select="attribute" mode="property"/>
@end
===========================<xsl:value-of select="@name"/>.h===========================
  </xsl:template>
  <xsl:template match="DailyUpdate/class" mode="m">
---------------------------<xsl:value-of select="@name"/>.m---------------------------
#import "<xsl:value-of select="@name"/>.h"
#import "Utils.h"
@implementation <xsl:value-of select="@name"/>
+(NSString *)tableName
{
    return @"<xsl:value-of select="@tableName"/>";
}
+(BOOL)createTable
{
    return [[[Database sharedInstance] sqlite] executeUpdate:[NSString stringWithFormat:
    @"CREATE TABLE IF NOT EXISTS %@ (id VARCHAR(36) PRIMARY KEY NOT NULL, "
    <xsl:apply-templates select="attribute" mode="createTable"/>
    @"new_record BOOL NOT NULL DEFAULT 0, "
    @"touched BOOL NOT NULL DEFAULT 0, "
    @"deleted BOOL NOT NULL DEFAULT 0, "
    @"created_at DATE NULL, "
    @"uploaded_at DATE NULL, "
    @"updated_at DATE NULL) ",
    [self tableName]]];
}

+(NSDate *)getMaxModDate
{
    NSDate *maxDate = nil;
    FMResultSet *rs = [[[Database sharedInstance] sqlite] executeQuery:[NSString stringWithFormat:@"SELECT max(updated_at) maxmod FROM %@", [self tableName]]];
    if ([rs next]) {
        maxDate = [rs dateForColumn:@"maxmod"];
    }
    [rs close];
    return maxDate;
}

+(<xsl:value-of select="@name"/> *)buildObject:(FMResultSet *)rs
{
    <xsl:value-of select="@name"/>*obj = [[<xsl:value-of select="@name"/> alloc] init];
    [obj refresh:rs];
    return obj;
}

-(id)proxyForJson
{
    // NOTE: not returning downloaded (or touched) as these are internal VM use
    // Also we turn into JSON following these rules ... if the checklist is new, return all
    // items, if it is not new return only the touched items.  This allows 'per item save'
    // so if user A and user B are editing different parts of the same, existing checklist,
    // the changes will merge.  Conflicts are simply last in is the winner.
    return @{<xsl:apply-templates select="attribute" mode="proxyForJson"/>};
}

+(id)fromJson:(NSDictionary*)obj insertRow:(BOOL)insertRow
{
    <xsl:value-of select="@name"/>* du = [<xsl:value-of select="@name"/> createObject:[obj valueForKey:@"id"]];
    <xsl:apply-templates select="attribute" mode="fromJson_insertRow_"/> 
   
    //for uploadable
    du.updatedAt = [Utils stringToDate:[obj valueForKey:@"updated_at"]];
    du.createdAt = [Utils stringToDate:[obj valueForKey:@"created_at"]];
    du.uploadedAt = nil;
    du.newRecord = FALSE;
    du.touched = FALSE;
    du.deleted = FALSE;
    
    [du save:FALSE];
    
    return du;
}

+(id)fromJson:(NSDictionary *)obj
{
    return [self fromJson:obj insertRow:TRUE];
}

-(void)refresh:(FMResultSet *)rs
{
    self.uuid = [rs stringForColumn:@"id"];
    <xsl:apply-templates select="attribute" mode="refresh_"/> 
    [super refresh:rs];
}

- (BOOL)save:(BOOL)setTouched
{
    NSString *sql = [NSString stringWithFormat:
    @"UPDATE %@ SET <xsl:apply-templates select="attribute" mode="save_UPDATE_SET"/> WHERE id = ?",
    [[self class] tableName]];
    self.touched = setTouched;
    
    Database *db = [Database sharedInstance];
    bool saved= [[db sqlite] executeUpdate:sql, <xsl:apply-templates select="attribute" mode="save_executeUpdate"/>] &amp;&amp; [super save:setTouched];
    
    return saved;
}

-(BOOL)save
{
    return [self save:TRUE];
}

-(BOOL)deleteRow {
    NSLog(@"Notice: Markup %@ is being deleted", self.uuid);
    
    return [super deleteRow];
}

@end
===========================<xsl:value-of select="@name"/>.m===========================
  </xsl:template>
  <xsl:template match="/DailyUpdate">
    <xsl:apply-templates select="class" mode="h"/>
    <xsl:apply-templates select="class" mode="m"/>
  </xsl:template>
  <!-- -->
  <xsl:template match="attribute" mode="property">
@property(<xsl:value-of select="@objc_p"/>)<xsl:value-of select="@objc_t"/><xsl:text> </xsl:text><xsl:value-of select="@name"/>;
  </xsl:template>
  <xsl:template match="attribute" mode="createTable">
    @"<xsl:value-of select="@name"/><xsl:text> </xsl:text><xsl:value-of select="@sql_t"/>, "
  </xsl:template>
  <xsl:template match="attribute" mode="proxyForJson">
    <xsl:choose>
      <xsl:when test="@objc_t = 'NSString*'">
    @"<xsl:value-of select="@name"/>":nil == self.<xsl:value-of select="@name"/>?@"":self.<xsl:value-of select="@name"/><xsl:if test="position()!=last()">,</xsl:if>
      </xsl:when>
      <xsl:when test="@objc_t = 'CGFloat'">
    @"<xsl:value-of select="@name"/>":[NSNumber numberWithFloat:self.<xsl:value-of select="@name"/>]<xsl:if test="position()!=last()">,</xsl:if>
      </xsl:when>
      <xsl:otherwise>
#error: Unknown type <xsl:value-of select="@objc_t"/> for variable <xsl:value-of select="@name"/> 
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="attribute" mode="fromJson_insertRow_">
    <xsl:choose>
      <xsl:when test="@objc_t = 'NSString*'">
    du.<xsl:value-of select="@name"/> = [obj valueForKey:@"<xsl:value-of select="@name"/>"];
      </xsl:when>
      <xsl:when test="@objc_t = 'NSDate*'">
    du.<xsl:value-of select="@name"/> = [Utils stringToDate:[obj valueForKey:@"<xsl:value-of select="@name"/>"]];
      </xsl:when>
      <xsl:when test="@objc_t = 'NSURL*'">
    du.<xsl:value-of select="@name"/> = [NSURL URLWithString:[obj valueForKey:@"<xsl:value-of select="@name"/>"]];
      </xsl:when>
      <xsl:when test="@objc_t = 'CGFloat'">
    du.<xsl:value-of select="@name"/> = [[obj valueForKey:@"<xsl:value-of select="@name"/>"] floatValue];
      </xsl:when>
      <xsl:otherwise>
#error: Unknown type <xsl:value-of select="@objc_t"/> for variable <xsl:value-of select="@name"/> 
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="attribute" mode="refresh_">
    <xsl:choose>
      <xsl:when test="@objc_t = 'NSString*'">
    self.<xsl:value-of select="@name"/> = [rs stringForColumn:@"<xsl:value-of select="@name"/>"];
      </xsl:when>
      <xsl:when test="@objc_t = 'NSDate*'">
    self.<xsl:value-of select="@name"/> = [rs dateForColumn:@"<xsl:value-of select="@name"/>"];
      </xsl:when>
      <xsl:when test="@objc_t = 'CGFloat'">
    self.<xsl:value-of select="@name"/> = [rs doubleForColumn:@"<xsl:value-of select="@name"/>"];
      </xsl:when>
      <xsl:otherwise>
#error: Unknown type <xsl:value-of select="@objc_t"/> for variable <xsl:value-of select="@name"/> 
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="attribute" mode="save_UPDATE_SET"><xsl:value-of select="@name"/> = ?<xsl:if test="position()!=last()">,</xsl:if></xsl:template>
  <xsl:template match="attribute" mode="save_executeUpdate"> self.<xsl:value-of select="@name"/><xsl:if test="position()!=last()">,</xsl:if></xsl:template>
</xsl:stylesheet>
